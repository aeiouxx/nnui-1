#include "maze-panel.h"

#include <wx/dcbuffer.h>

#include "wx/gdicmn.h"
#include "wx/log.h"

namespace astar::ui {
using astar::common::CellType;
using astar::common::Grid;
const wxPoint MazePanel::kInvalidCell{-1, -1};
MazePanel::MazePanel(wxWindow* parent, wxColour backgroundColour)
    : wxPanel(parent),
      grid_(0, 0),
      resizeDebouncer_(this),
      zoomFactor_(1.0f),
      panOffset_(0, 0),
      ctrlDown_(false),
      lastHoveredCell_(kInvalidCell),
      lastMousePosition_(wxDefaultPosition) {
  SetBackgroundColour(backgroundColour);
  BindEvents();
}
MazePanel::~MazePanel() {
}
void MazePanel::BindEvents() {
  Bind(wxEVT_PAINT, &MazePanel::OnPaint, this);
  Bind(wxEVT_SIZE, &MazePanel::OnResize, this);
  Bind(wxEVT_TIMER, &MazePanel::OnResizeTimer, this, resizeDebouncer_.GetId());
  // Mouse
  Bind(wxEVT_MOTION, &MazePanel::OnMouseMove, this);
  Bind(wxEVT_MOUSEWHEEL, &MazePanel::OnMouseWheel, this);
  Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& e) {
    wxLogDebug("OnLeftDown");
    lastMousePosition_ = e.GetPosition();
    e.Skip();
  });
  Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& e) {
    wxLogDebug("OnLeftUp");
    lastMousePosition_ = wxDefaultPosition;
    e.Skip();
  });
  // Keyboard
  Bind(wxEVT_KEY_DOWN, &MazePanel::OnKeyDown, this);
  Bind(wxEVT_KEY_UP, &MazePanel::OnKeyUp, this);

  // do nothing to prevent flickering
  Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
}
void MazePanel::SetGrid(const Grid& grid) {
  grid_ = grid;
  UpdateSizeInformation();
}
void MazePanel::SetGrid(Grid&& grid) noexcept {
  grid_ = std::move(grid);
  UpdateSizeInformation();
}
const Grid& MazePanel::GetGrid() const {
  return grid_;
}
Grid& MazePanel::GetGrid() {
  return grid_;
}
// Event handling >
void MazePanel::OnPaint(wxPaintEvent& event) {
  wxBufferedPaintDC dc(this);
  Render(dc);
}
void MazePanel::OnResize(wxSizeEvent& event) {
  shouldRenderGrid_ = false;
  resizeDebouncer_.Start(500, true);
}
void MazePanel::OnResizeTimer(wxTimerEvent&) {
  wxLogDebug("OnResizeTimer");
  shouldRenderGrid_ = true;
  UpdateSizeInformation();
  Refresh();
  Update();
}
void MazePanel::OnMouseMove(wxMouseEvent& event) {
  wxPoint mousePosition = event.GetPosition();
  if (ctrlDown_) {
    if (event.LeftIsDown()) {
      HandleDrag(mousePosition);
    }
  } else {
    UpdateCursorAndInteractions(mousePosition);
  }
  event.Skip();
}
void MazePanel::HandleDrag(const wxPoint& mousePosition) {
  if (lastMousePosition_ != wxDefaultPosition) {
    const auto delta = mousePosition - lastMousePosition_;
    panOffset_ += delta;
    ConstrainPanOffset();
    Refresh();
  }
  lastMousePosition_ = mousePosition;
}
void MazePanel::UpdateCursorAndInteractions(const wxPoint& mousePosition) {
  auto cell = GetCellFromMousePosition(mousePosition);
  if (cell != kInvalidCell && grid_.IsTraversable(cell.y, cell.x)) {
    SetCursor(wxCURSOR_ARROW);
    if (lastHoveredCell_ != cell) {
      UpdateHoveredCell(cell);
    }
  } else {
    SetCursor(wxCURSOR_NO_ENTRY);
    if (lastHoveredCell_ != kInvalidCell) {
      UpdateHoveredCell(kInvalidCell);
    }
  }
}
void MazePanel::UpdateHoveredCell(const wxPoint& cell) {
  wxClientDC dc(this);
  wxBufferedDC bufferedDC(&dc);
  const auto oldCell = lastHoveredCell_;
  lastHoveredCell_ = cell;
  if (oldCell != kInvalidCell) {
    RenderCell(bufferedDC, oldCell);
  }
  if (lastHoveredCell_ != kInvalidCell) {
    RenderCell(bufferedDC, lastHoveredCell_);
  }
}
void MazePanel::OnMouseWheel(wxMouseEvent& event) {
  static constexpr int pixelZoomIncrement = 6;
  wxLogDebug("OnMouseWheel");
  if (!ctrlDown_ || grid_.IsEmpty()) {
    return;
  }
  int oldCellSize = cellSize_;
  auto mousePosition = event.GetPosition();
  if (event.GetWheelRotation() > 0) {
    cellSize_ += pixelZoomIncrement;
  } else {
    cellSize_ -= pixelZoomIncrement;
  }
  cellSize_ = std::max(cellSize_, kMinimumCellSize);
  double scaleFactor = static_cast<double>(cellSize_) / oldCellSize;

  // new offset
  wxPoint cellBeforeZoom = (mousePosition - panOffset_) / oldCellSize;
  wxPoint newMouseCell = mousePosition / oldCellSize;
  wxPoint newOffset = mousePosition - newMouseCell * cellSize_;
  panOffset_ = panOffset_ * scaleFactor + newOffset;
  Refresh();
}

void MazePanel::OnKeyDown(wxKeyEvent& event) {
  wxLogDebug("OnKeyDown");
  switch (event.GetKeyCode()) {
    case WXK_CONTROL:
      ctrlDown_ = true;
      SetCursor(wxCURSOR_SIZING);
      break;
    default:
      event.Skip();
  }
}
void MazePanel::OnKeyUp(wxKeyEvent& event) {
  wxLogDebug("OnKeyUp");
  switch (event.GetKeyCode()) {
    case WXK_CONTROL: {
      ctrlDown_ = false;
    } break;
    default:
      event.Skip();
  }
}

// < Event handling
// Rendering >
void MazePanel::Render(wxDC& dc) {
  dc.Clear();
  if (!shouldRenderGrid_) {
    return;
  }
  wxRect visiblePortion = GetVisiblePortion();
  for (int row = visiblePortion.GetTop(); row <= visiblePortion.GetBottom();
       row++) {
    for (int col = visiblePortion.GetLeft(); col <= visiblePortion.GetRight();
         col++) {
      int y = row * cellSize_ + panOffset_.y;
      int x = col * cellSize_ + panOffset_.x;
      int size = cellSize_ * zoomFactor_;

      wxColour cellColour = GetCellColour(grid_.At(row, col));
      dc.SetBrush(wxBrush(cellColour));
      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.DrawRectangle(x, y, size, size);
    }
  }
}
void MazePanel::RenderCell(wxDC& dc, const wxPoint& cell) {
  int x = cell.x * cellSize_ + panOffset_.x;
  int y = cell.y * cellSize_ + panOffset_.y;
  int size = cellSize_ * zoomFactor_;
  wxColour cellColour = GetCellColour(grid_.At(cell.y, cell.x));
  if (cell == lastHoveredCell_) {
    cellColour = wxColour(123, 22, 222, 255);
  }
  dc.SetBrush(wxBrush(cellColour));
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.DrawRectangle(x, y, size, size);
}
// < Rendering
// Utilities >
wxRect MazePanel::GetVisiblePortion() const {
  wxSize panelSize = GetClientSize();
  int visibleLeft = std::max(0, -panOffset_.x / cellSize_);
  int visibleTop = std::max(0, -panOffset_.y / cellSize_);
  int visibleRight = std::min(
      grid_.GetCols(), (panelSize.GetWidth() - panOffset_.x) / cellSize_ + 1);
  int visibleBottom = std::min(
      grid_.GetRows(), (panelSize.GetHeight() - panOffset_.y) / cellSize_ + 1);

  return {visibleLeft, visibleTop, visibleRight - visibleLeft,
          visibleBottom - visibleTop};
}
void MazePanel::UpdateSizeInformation() {
  if (grid_.IsEmpty()) {
    return;
  }
  wxSize size = GetClientSize();
  double screenAspectRatio = static_cast<double>(size.GetWidth()) /
                             static_cast<double>(size.GetHeight());
  double gridAspectRatio = static_cast<double>(grid_.GetCols()) /
                           static_cast<double>(grid_.GetRows());
  int maximumCellWidth = size.GetWidth() / grid_.GetCols();
  int maximumCellHeight = size.GetHeight() / grid_.GetRows();
  cellSize_ = std::max(MazePanel::kMinimumCellSize,
                       std::min(maximumCellWidth, maximumCellHeight));

  // check if we can't fit the entire grid on the screen
  if (screenAspectRatio > gridAspectRatio) {
    cellSize_ = std::min(cellSize_, size.GetHeight() / grid_.GetRows());
  } else {
    cellSize_ = std::min(cellSize_, size.GetWidth() / grid_.GetCols());
  }

  ConstrainPanOffset();
}
wxPoint MazePanel::GetCellFromMousePosition(
    const wxPoint& mousePosition) const {
  int column = (mousePosition.x - panOffset_.x) / cellSize_;
  int row = (mousePosition.y - panOffset_.y) / cellSize_;
  if (row < 0 || row >= grid_.GetRows() || column < 0 ||
      column >= grid_.GetCols()) {
    return {-1, -1};
  }
  wxLogDebug("GetCellFromMousePosition (x, y): %d, %d", mousePosition.x,
             mousePosition.y);
  wxLogDebug("GetCellFromMousePosition (row, column): %d, %d", row, column);
  return {column, row};
}
wxColour MazePanel::GetCellColour(const common::CellType& cellType) const {
  // todo: Use a map instead of a switch
  static const wxColour kEmptyCellColour = wxColour(255, 255, 255);
  static const wxColour kWallCellColour = wxColour(0, 0, 0);
  static const wxColour kStartCellColour = wxColour(255, 0, 0);
  static const wxColour kGoalCellColour = wxColour(0, 255, 0);
  switch (cellType) {
    case CellType::kEmpty:
      return kEmptyCellColour;
    case CellType::kWall:
      return kWallCellColour;
    case CellType::kStart:
      return kStartCellColour;
    case CellType::kGoal:
      return kGoalCellColour;
    default:
      return kEmptyCellColour;
  }
}
void MazePanel::ConstrainPanOffset() {
  auto panelSize = GetClientSize();

  int maximumPanOffsetX =
      std::max(0, grid_.GetCols() * cellSize_ - panelSize.GetWidth());
  int maximumPanOffsetY =
      std::max(0, grid_.GetRows() * cellSize_ - panelSize.GetHeight());

  panOffset_.x = std::min(0, panOffset_.x);
  panOffset_.y = std::min(0, panOffset_.y);

  panOffset_.x = std::max(-maximumPanOffsetX, panOffset_.x);
  panOffset_.y = std::max(-maximumPanOffsetY, panOffset_.y);
}
};  // namespace astar::ui
