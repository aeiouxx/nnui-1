#ifndef ASTAR_PATHFINDING_ALGORITHM_H
#define ASTAR_PATHFINDING_ALGORITHM_H
#include <limits>
#include <vector>

#include "../common/definitions.h"
#include "../common/grid.h"
// Initial implementation of the A* algorithm.
// TODO: Will need a mechanism to
// queue updates on the UI thread, will need to support cancellation for
// threading.
namespace astar::pathfinding {
using astar::common::ActionCost;
using astar::common::Grid;
using astar::common::Orientation;
using astar::common::Position;
struct Node {
  Position position = Position::kInvalid;
  Orientation orientation = Orientation::kNorth;
  int g_cost = std::numeric_limits<int>::max();
  int f_cost = std::numeric_limits<int>::max();

  Node *parent = nullptr;
};
struct CompareNode {
  bool operator()(const Node &lhs, const Node &rhs) const;
};
// Group into class, can also track cancellation request state,
// without having to pass it around as a parameter.
std::vector<Node> RunAstar(const Grid &grid_snapshot, const Position &start,
                           const Orientation &start_orientation,
                           const Position &goal);

std::vector<Node> ReconstructPath(const Node &goal);

Node CalculateNextState(const Node &current, const Orientation &orientation,
                        const Grid &grid_snapshot, const Position &goal);
int GetTurnCost(const Orientation &current, const Orientation &goal);
int CalculateHeuristic(const Position &start,
                       const Orientation &start_orientation,
                       const Position &goal);
}  // namespace astar::pathfinding
#endif
