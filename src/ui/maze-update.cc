#include "maze-update.h"
namespace astar::ui {
wxDEFINE_EVENT(myEVT_MAZE_UPDATE, MazeUpdateEvent);
MazeUpdate::MazeUpdate(Position position, CellType cell_type)
    : position(position), cell_type(cell_type) {
}
MazeUpdateEvent::MazeUpdateEvent(wxEventType eventType, int id)
    : wxCommandEvent(eventType, id) {
}
MazeUpdateEvent::MazeUpdateEvent(const MazeUpdateEvent &event)
    : wxCommandEvent(event) {
  updates_ = event.updates_;
};
void MazeUpdateEvent::SetUpdates(const std::vector<MazeUpdate> &updates) {
  updates_ = updates;
}
std::vector<MazeUpdate> MazeUpdateEvent::GetUpdates() const {
  return updates_;
}
wxEvent *MazeUpdateEvent::Clone() const {
  return new MazeUpdateEvent(*this);
}
}  // namespace astar::ui
