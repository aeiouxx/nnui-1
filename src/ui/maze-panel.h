#ifndef ASTAR_MAZE_PANEL_H
#define ASTAR_MAZE_PANEL_H

#include <wx/panel.h>
#include <wx/timer.h>

#include <mutex>
#include <thread>

#include "../common/grid.h"
#include "maze-update.h"
namespace astar::ui {
class MazePanel : public wxPanel {
 private:
  static constexpr int kMinimumCellSize = 36;
  static const wxPoint kInvalidCell;

 public:
  MazePanel(wxWindow* parent, wxColour backgroundColor);
  ~MazePanel();
  void SetGrid(const astar::common::Grid& grid);
  void SetGrid(astar::common::Grid&& grid) noexcept;
  const common::Grid& GetGrid() const;
  common::Grid& GetGrid();

 protected:
  void BindEvents();
  void OnPaint(wxPaintEvent& event);
  void OnResize(wxSizeEvent& event);
  void OnResizeTimer(wxTimerEvent& event);

  // Mouse
  void OnMouseMove(wxMouseEvent& event);
  void OnMouseWheel(wxMouseEvent& event);
  void OnMouseClick(wxMouseEvent& event);
  void HandleDrag(const wxPoint& mousePosition);
  void UpdateCursorAndInteractions(const wxPoint& mousePosition);
  // Keyboard
  void OnKeyDown(wxKeyEvent& event);
  void OnKeyUp(wxKeyEvent& event);
  // Rendering
  void Render(wxDC& dc);
  void RenderCell(wxDC& dc, const wxPoint& cell);
  void OnMazeUpdate(MazeUpdateEvent& event);
  // Utilities
  wxRect GetVisiblePortion() const;
  void UpdateSizeInformation();
  wxBrush GetCellBrush(const common::CellType& cellType) const;
  wxPoint GetCellFromMousePosition(const wxPoint& mousePosition) const;
  void UpdateHoveredCell(const wxPoint& cell);

 private:
  astar::common::Grid grid_;
  wxTimer resizeDebouncer_;

  // RenderInfo
  wxPoint panOffset_;
  wxPoint lastMousePosition_;
  wxPoint lastHoveredCell_;
  double zoomFactor_;
  bool ctrlDown_;
  bool shouldRenderGrid_;
  int cellSize_;

  // Pathfinding
  std::thread pathfindingThread_;
  std::atomic_bool pathfindingThreadRunning_;
  std::atomic_bool pathfindingThreadShouldStop_;
  std::mutex gridMutex_;
};
}  // namespace astar::ui
#endif
