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

  // todo: in cc file
  Position GetPosition() const {
    return position;
  }
  CellType GetCellType() const {
    return cell_type;
  }
};
// needs forward declaration for macro.
class MazeUpdateEvent;
wxDECLARE_EVENT(myEVT_MAZE_UPDATE, MazeUpdateEvent);
class MazeUpdateEvent : public wxCommandEvent {
 public:
  MazeUpdateEvent(wxEventType eventType = myEVT_MAZE_UPDATE, int id = 0);
  MazeUpdateEvent(const MazeUpdateEvent& event);
  void SetUpdates(const std::vector<MazeUpdate>& updates);
  std::vector<MazeUpdate> GetUpdates() const;

  // Kind of stupid but clone takes ownership of the
  // MazeUpdates, definitely ugly.
  wxEvent* Clone() const override;

 private:
  std::vector<MazeUpdate> updates_;
};
// typedef void (wxEvtHandler::*MazeUpdateEventFunction)(MazeUpdateEvent&);
};  // namespace astar::ui
#endif
