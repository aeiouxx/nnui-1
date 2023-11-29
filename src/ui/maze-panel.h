#ifndef ASTAR_MAZE_PANEL_H
#define ASTAR_MAZE_PANEL_H

#include <wx/panel.h>

#include <atomic>
#include <chrono>

#include "../common/grid.h"
#include "wx/timer.h"
namespace astar::ui {
// todo: if we want to support zoom / pan / mouse hover, will need more
// sophisticated rendering + event handling. (or could just render the entire
// grid and use a scrolled window xdd)
class MazePanel : public wxPanel {
 public:
  MazePanel(wxWindow* parent);
  void SetGrid(const astar::common::Grid& grid);
  void SetGrid(astar::common::Grid&& grid) noexcept;
  const common::Grid& GetGrid() const;
  common::Grid& GetGrid();
  void Refresh();
  // protected:
  //  void OnPaint(wxPaintEvent& event);
  //  void OnResize(wxSizeEvent& event);
  //  void OnMouseClick(wxMouseEvent& event);
  //  void OnMouseMove(wxMouseEvent& event);
  //  void OnMouseScroll(wxMouseEvent& event);
  //  void OnMouseDrag(wxMouseEvent& event);
  //  void Render(wxDC& dc);
 protected:
  void BindEvents();
  void OnResize(wxSizeEvent& event);
  void OnResizeTimer(wxTimerEvent& event);
  void RefreshInternal();

 private:
  astar::common::Grid grid_;
  std::atomic_bool isRedrawing_;
  // indicates that we received a refresh request while we were
  // already in the process of redrawing.
  std::atomic_bool isRedrawPending_;
  wxTimer resizeDebouncer_;
};
}  // namespace astar::ui
#endif
