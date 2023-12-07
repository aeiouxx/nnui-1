#ifndef ASTAR_MAZE_PANEL_H
#define ASTAR_MAZE_PANEL_H

#include <wx/panel.h>
#include <wx/timer.h>

#include <mutex>
#include <thread>

#include "../common/grid.h"
#include "../pathfinding/algorithm.h"
#include "maze-update.h"
namespace astar::ui {
class MazeCanvas : public wxPanel {
 private:
  static constexpr int kMinimumCellSize = 36;
  static const wxPoint kInvalidCell;
  static constexpr int kResizeDebounceTime = 250;

 public:
  MazeCanvas(wxWindow *parent, wxColour background_color);
  ~MazeCanvas();
  void SetGrid(const astar::common::Grid &grid);
  void SetGrid(astar::common::Grid &&grid) noexcept;
  const common::Grid &GetGrid() const;
  common::Grid &GetGrid();

 private:
  void BindEvents();
  void OnPaint(wxPaintEvent &event);
  void OnResize(wxSizeEvent &event);
  void OnResizeTimer(wxTimerEvent &event);
  void OnMouseMove(wxMouseEvent &event);
  void OnMouseWheel(wxMouseEvent &event);
  void OnMouseClick(wxMouseEvent &event);
  void HandleDrag(const wxPoint &mouse_position);
  void UpdateCursorAndInteractions(const wxPoint &mouse_position);
  void OnKeyDown(wxKeyEvent &event);
  void OnKeyUp(wxKeyEvent &event);
  void OnMazeUpdate(MazeUpdateEvent &event);
  void Render(wxDC &dc);
  void RenderCell(wxDC &dc, const wxPoint &cell);
  wxRect GetVisiblePortion() const;
  void UpdateSizeInformation();
  wxBrush GetCellBrush(const common::CellType &cell_type,
                       bool is_hovered = false) const;
  wxPoint GetCellFromMousePosition(const wxPoint &mouse_position) const;
  void UpdateHoveredCell(const wxPoint &cell);

  void MaybeRunPathfinding();

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
  pathfinding::AstarAlgorithm pathfindingAlgorithm_;
  std::thread pathfindingThread_;
  std::mutex gridMutex_;
};
}  // namespace astar::ui
#endif
