#ifndef ASTAR_UI_MAZE_UPDATE_H
#define ASTAR_UI_MAZE_UPDATE_H
#include <wx/event.h>

#include "../common/definitions.h"
#include "../common/grid.h"
namespace astar::ui {
using astar::common::CellType;
using astar::common::Grid;
using astar::common::Orientation;
using astar::common::Position;
struct MazeUpdate {
  Position position;
  CellType cell_type;
  MazeUpdate(Position position, CellType cell_type);
  Position GetPosition() const {
    return position;
  }
  CellType GetCellType() const {
    return cell_type;
  }
};
class MazeUpdateEvent;
wxDECLARE_EVENT(myEVT_MAZE_UPDATE, MazeUpdateEvent);
class MazeUpdateEvent : public wxCommandEvent {
 public:
  MazeUpdateEvent(wxEventType event_type = myEVT_MAZE_UPDATE, int id = 0);
  MazeUpdateEvent(const MazeUpdateEvent &event);
  void SetUpdates(const std::vector<MazeUpdate> &updates);
  std::vector<MazeUpdate> GetUpdates() const;

  // should not be taking ownership of the vector, but it do.
  wxEvent *Clone() const override;

 private:
  std::vector<MazeUpdate> updates_;
};
};  // namespace astar::ui
#endif
