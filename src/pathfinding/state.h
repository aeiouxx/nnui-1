#ifndef ASTAR_PATHFINDING_STATE_H
#define ASTAR_PATHFINDING_STATE_H
#include <functional>
#include <vector>

#include "../common/definitions.h"
#include "../common/grid.h"
// TODO: Encapsulate behaviour in a single class.
namespace astar::pathfinding {
using astar::common::CellType;
using astar::common::Grid;
using astar::common::Orientation;
using astar::common::Position;
struct AgentState {
  Position position;
  Orientation orientation;

  AgentState(Position position, Orientation orientation);
  bool operator==(const AgentState& other) const;
  bool operator!=(const AgentState& other) const;

  // TODO: could implement a hash function to search closedSet faster (or could
  // modify the heuristic to not even need the closed set).
};
// FIXME: inefficient transformation of grid to nodes, is there a better way to
// do
struct Node {
  AgentState state;
  Node* parent;
  int g_cost;
  int h_cost;
  int f_cost;
  Node(AgentState state, Node* parent, int g_cost, int h_cost);
};
/*
 * TODO: We have no direct way to clear our maze update event queue,
 * maybe each event should have a timestamp and we can ignore update events
 * emitted before the cancellation request timestamp, will still require
 * iterating the event queue but wcyd?
 *
 */
}  // namespace astar::pathfinding
#endif
