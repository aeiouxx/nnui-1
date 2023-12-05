#include "maze-panel.h"

#include <wx/dcbuffer.h>

#include <thread>

#include "maze-update.h"
#include "wx/event.h"
#include "wx/gdicmn.h"
#include "wx/log.h"

namespace astar::ui {
using astar::common::CellType;
using astar::common::Grid;
const wxPoint MazeCanvas::kInvalidCell{-1, -1};
MazeCanvas::MazeCanvas(wxWindow *parent, wxColour backgroundColour)
    : wxPanel(parent),
      grid_(0, 0),
      resizeDebouncer_(this),
      zoomFactor_(1.0f),
      panOffset_(0, 0),
      ctrlDown_(false),
      lastHoveredCell_(kInvalidCell),
      lastMousePosition_(wxDefaultPosition),
      cellSize_(kMinimumCellSize) {
  SetBackgroundColour(backgroundColour);
  BindEvents();
}
MazeCanvas::~MazeCanvas() {
  pathfindingThreadShouldStop_ = true;
  if (pathfindingThread_.joinable()) {
    pathfindingThread_.join();
  }
}
void MazeCanvas::BindEvents() {
  Bind(wxEVT_PAINT, &MazeCanvas::OnPaint, this);
  Bind(wxEVT_SIZE, &MazeCanvas::OnResize, this);
  Bind(wxEVT_TIMER, &MazeCanvas::OnResizeTimer, this, resizeDebouncer_.GetId());
  // Mouse
  Bind(wxEVT_MOTION, &MazeCanvas::OnMouseMove, this);
  Bind(wxEVT_MOUSEWHEEL, &MazeCanvas::OnMouseWheel, this);
  Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent &e) {
    wxLogDebug("OnLeftDown");
    lastMousePosition_ = e.GetPosition();
    e.Skip();
  });
  Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &e) {
    // Can also toggle direction of start if we click on it.
    wxLogDebug("OnLeftUp");
    lastMousePosition_ = wxDefaultPosition;
    if (!ctrlDown_) {
      auto cell = GetCellFromMousePosition(e.GetPosition());
      common::Position startPosition = grid_.GetStart();
      wxPoint oldStart = wxPoint(startPosition.col, startPosition.row);
      if (cell != kInvalidCell && grid_.IsTraversable(cell.y, cell.x)) {
        auto dc = wxClientDC(this);
        grid_.ToggleStart(cell.y, cell.x);
        if (oldStart != kInvalidCell && oldStart != cell) {
          wxLogDebug("Redrawing old start position %d, %d", oldStart.x,
                     oldStart.y);
          wxLogDebug("New start position is %d, %d", cell.x, cell.y);
          RenderCell(dc, oldStart);
        }
        RenderCell(dc, cell);
      }
    }
    e.Skip();
  });
  Bind(wxEVT_RIGHT_DOWN, [this](wxMouseEvent &e) {
    wxLogDebug("OnRightDown");
    lastMousePosition_ = e.GetPosition();
    if (!ctrlDown_) {
      auto cell = GetCellFromMousePosition(e.GetPosition());
      common::Position goalPosition = grid_.GetGoal();
      wxPoint oldGoal = wxPoint(goalPosition.col, goalPosition.row);
      if (cell != kInvalidCell && grid_.IsTraversable(cell.y, cell.x)) {
        auto dc = wxClientDC(this);
        grid_.ToggleGoal(cell.y, cell.x);
        if (oldGoal != kInvalidCell && oldGoal != cell) {
          wxLogDebug("Redrawing old goal position %d, %d", oldGoal.x,
                     oldGoal.y);
          wxLogDebug("New goal position is %d, %d", cell.x, cell.y);
          RenderCell(dc, oldGoal);
        }
        RenderCell(dc, cell);
      }
    }
    e.Skip();
  });
  // Keyboard
  Bind(wxEVT_KEY_DOWN, &MazeCanvas::OnKeyDown, this);
  Bind(wxEVT_KEY_UP, &MazeCanvas::OnKeyUp, this);
  // do nothing to prevent flickering
  Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent &) {});
  Bind(myEVT_MAZE_UPDATE, &MazeCanvas::OnMazeUpdate, this);
}
void MazeCanvas::SetGrid(const Grid &grid) {
  grid_ = grid;
  panOffset_ = wxDefaultPosition;
  zoomFactor_ = 1.0f;
  UpdateSizeInformation();
}
void MazeCanvas::SetGrid(Grid &&grid) noexcept {
  grid_ = std::move(grid);
  UpdateSizeInformation();
}
const Grid &MazeCanvas::GetGrid() const {
  return grid_;
}
Grid &MazeCanvas::GetGrid() {
  return grid_;
}
// Event handling >
void MazeCanvas::OnPaint(wxPaintEvent &event) {
  wxBufferedPaintDC dc(this);
  Render(dc);
}
void MazeCanvas::OnResize(wxSizeEvent &event) {
  shouldRenderGrid_ = false;
  resizeDebouncer_.Start(500, true);
}
void MazeCanvas::OnResizeTimer(wxTimerEvent &) {
  wxLogDebug("OnResizeTimer");
  shouldRenderGrid_ = true;
  UpdateSizeInformation();
  Refresh();
  Update();
}
void MazeCanvas::OnMouseMove(wxMouseEvent &event) {
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
void MazeCanvas::HandleDrag(const wxPoint &mousePosition) {
  if (lastMousePosition_ != wxDefaultPosition) {
    const auto delta = mousePosition - lastMousePosition_;
    panOffset_ += delta;
    Refresh();
  }
  lastMousePosition_ = mousePosition;
}
void MazeCanvas::UpdateCursorAndInteractions(const wxPoint &mousePosition) {
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
void MazeCanvas::UpdateHoveredCell(const wxPoint &cell) {
  wxClientDC dc(this);
  const auto oldCell = lastHoveredCell_;
  lastHoveredCell_ = cell;
  if (oldCell != kInvalidCell) {
    RenderCell(dc, oldCell);
  }
  if (lastHoveredCell_ != kInvalidCell) {
    RenderCell(dc, lastHoveredCell_);
  }
}
void MazeCanvas::OnMouseWheel(wxMouseEvent &event) {
  static constexpr int pixelZoomIncrement = 4;
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
    cellSize_ = std::max(1, cellSize_);
  }
  double scaleFactor = static_cast<double>(cellSize_) / oldCellSize;
  // new offset
  wxPoint cellBeforeZoom = (mousePosition - panOffset_) / oldCellSize;
  wxPoint newMouseCell = mousePosition / oldCellSize;
  wxPoint newOffset = mousePosition - newMouseCell * cellSize_;
  panOffset_ = panOffset_ * scaleFactor + newOffset;
  Refresh();
}

void MazeCanvas::OnKeyDown(wxKeyEvent &event) {
  wxLogDebug("OnKeyDown");
  switch (event.GetKeyCode()) {
    case WXK_CONTROL: {
      ctrlDown_ = true;
      auto oldHighlight = lastHoveredCell_;
      lastHoveredCell_ = kInvalidCell;
      if (oldHighlight != kInvalidCell) {
        auto dc = wxClientDC(this);
        RenderCell(dc, oldHighlight);
      }
      SetCursor(wxCURSOR_SIZING);
    } break;
    default:
      event.Skip();
  }
}
void MazeCanvas::OnKeyUp(wxKeyEvent &event) {
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
// TODO:
// Maybe try to use blitting, we will still need to redraw the entire canvas
// on zooming / panning.
void MazeCanvas::Render(wxDC &dc) {
  dc.Clear();
  if (!shouldRenderGrid_) {
    return;
  }
  wxRect visiblePortion = GetVisiblePortion();
  // this is not optimized at all
  // could use chunking to draw uniform areas within 1 draw call(e.g. one large
  // rectangle instead of many small ones, which would also cause use to change
  // brushes less often), dirty rects to only redraw
  // areas that actualy need redrawing,
  int startCount = 0;
  wxLogDebug("Rendering whole grid");
  for (int row = visiblePortion.GetTop(); row <= visiblePortion.GetBottom();
       row++) {
    for (int col = visiblePortion.GetLeft(); col <= visiblePortion.GetRight();
         col++) {
      int y = row * cellSize_ + panOffset_.y;
      int x = col * cellSize_ + panOffset_.x;
      int size = cellSize_ * zoomFactor_;
      const auto type = grid_.At(row, col);
#if ASTAR_DEBUG
      // got confused because I used a bufferedDC in RenderCell which redrew the
      // original start cell after It had already drawn the new one
      if (type == CellType::kStart && ++startCount > 1) {
        wxLogError("!!! Drawing duplicit start cell at %d, %d", col, row);
      }
#endif
      dc.SetBrush(GetCellBrush(type));
      dc.SetPen(*wxTRANSPARENT_PEN);
      dc.DrawRectangle(x, y, size, size);
    }
  }
}
void MazeCanvas::RenderCell(wxDC &dc, const wxPoint &cell) {
  int x = cell.x * cellSize_ + panOffset_.x;
  int y = cell.y * cellSize_ + panOffset_.y;
  int size = cellSize_ * zoomFactor_;
  if (cell == lastHoveredCell_) {
    dc.SetBrush(wxBrush(wxColour(192, 192, 192)));
  } else {
    dc.SetBrush(GetCellBrush(grid_.At(cell.y, cell.x)));
  }
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.DrawRectangle(x, y, size, size);
}
// should probably set up a debouncer / throttling for this too
void MazeCanvas::OnMazeUpdate(MazeUpdateEvent &event) {
  static int evtCounter = 0;
  wxLogDebug("OnMazeUpdate %d", evtCounter++);
  auto dc = wxClientDC(this);
  // once again, giga inefficient
  for (const auto update : event.GetUpdates()) {
    const auto pos = update.GetPosition();
    const auto type = update.GetCellType();
    wxLogDebug("Drawing update at %d, %d, cellType = %d", pos.col, pos.row,
               type);
    grid_.At(pos.row, pos.col) = type;
    RenderCell(dc, wxPoint(pos.col, pos.row));
  }
}
// < Rendering
// Utilities >
wxRect MazeCanvas::GetVisiblePortion() const {
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
void MazeCanvas::UpdateSizeInformation() {
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
  cellSize_ = std::max(MazeCanvas::kMinimumCellSize,
                       std::min(maximumCellWidth, maximumCellHeight));

  // check if we can't fit the entire grid on the screen
  if (screenAspectRatio > gridAspectRatio) {
    cellSize_ =
        std::max(1, std::min(cellSize_, size.GetHeight() / grid_.GetRows()));
  } else {
    cellSize_ =
        std::max(1, std::min(cellSize_, size.GetWidth() / grid_.GetCols()));
  }
}
wxPoint MazeCanvas::GetCellFromMousePosition(
    const wxPoint &mousePosition) const {
  int column = (mousePosition.x - panOffset_.x) / cellSize_;
  int row = (mousePosition.y - panOffset_.y) / cellSize_;
  if (row < 0 || row >= grid_.GetRows() || column < 0 ||
      column >= grid_.GetCols()) {
    return {-1, -1};
  }
  // wxLogDebug("GetCellFromMousePosition (x, y): %d, %d", mousePosition.x,
  //            mousePosition.y);
  // wxLogDebug("GetCellFromMousePosition (row, column): %d, %d", row, column);
  return {column, row};
}

// TODO: indexed color palette for
// normal cells and hovered cells...
wxBrush MazeCanvas::GetCellBrush(const common::CellType &cellType) const {
  static const wxBrush kEmptyCellBrush = wxBrush(wxColour(255, 255, 255));
  static const wxBrush kWallCellBrush = wxBrush(wxColour(0, 0, 0));
  static const wxBrush kStartCellBrush = wxBrush(wxColour(0, 255, 0));
  static const wxBrush kGoalCellBrush = wxBrush(wxColour(255, 0, 0));
  static const wxBrush kVisitedCellBrush = wxBrush(wxColour(0, 0, 255));

  switch (cellType) {
    case CellType::kEmpty:
      return kEmptyCellBrush;
    case CellType::kWall:
      return kWallCellBrush;
    case CellType::kStart:
      return kStartCellBrush;
    case CellType::kGoal:
      return kGoalCellBrush;
    case CellType::kVisited:
      return kVisitedCellBrush;
    default:
      return kEmptyCellBrush;
  }
}
}  // namespace astar::ui
