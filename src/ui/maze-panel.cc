#include "maze-panel.h"

#include <chrono>
namespace astar::ui {
using astar::common::CellType;
using astar::common::Grid;
MazePanel::MazePanel(wxWindow* parent)
    : wxPanel(parent), grid_(0, 0), resizeDebouncer_(this) {
  BindEvents();
}
void MazePanel::BindEvents() {
  Bind(wxEVT_SIZE, &MazePanel::OnResize, this);
  Bind(wxEVT_TIMER, &MazePanel::OnResizeTimer, this, resizeDebouncer_.GetId());
}
void MazePanel::SetGrid(const Grid& grid) { grid_ = grid; }
void MazePanel::SetGrid(Grid&& grid) noexcept { grid_ = std::move(grid); }
const Grid& MazePanel::GetGrid() const { return grid_; }
Grid& MazePanel::GetGrid() { return grid_; }
void MazePanel::OnResize(wxSizeEvent& event) {
  resizeDebouncer_.Start(1000, true);
}
void MazePanel::OnResizeTimer(wxTimerEvent&) {
  resizeDebouncer_.Stop();
  Refresh();
}
void MazePanel::Refresh() {
  bool expected = false;
  // Basically if we're already redrawing we set a flag to indicate that
  // we need to redraw again once we're done.
  // If we're not redrawing, then we set isRedrawing_ to true and redraw.
  if (!isRedrawing_.compare_exchange_strong(expected, true)) {
    isRedrawPending_ = true;
    return;
  }
  RefreshInternal();
  isRedrawing_ = false;
  if (isRedrawPending_.exchange(false)) {
    Refresh();
  }
}

void MazePanel::RefreshInternal() {
  //  wxPaintDC dc(this);
  //  Render(dc);
}
};  // namespace astar::ui
