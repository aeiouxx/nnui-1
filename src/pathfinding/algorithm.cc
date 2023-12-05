#include "algorithm.h"

#include <map>
#include <queue>

namespace astar::pathfinding {
using astar::common::CellType;
bool CompareNode::operator()(const Node &lhs, const Node &rhs) const {
  return lhs.f_cost > rhs.f_cost;
}

// fixme: NOT THREAD SAFE, will need to be refactored.
std::vector<Node> RunAstar(const Grid &grid, const Position &start,
                           const Orientation &start_orientation,
                           const Position &goal) {
  const Orientation orientations[] = {Orientation::kNorth, Orientation::kEast,
                                      Orientation::kSouth, Orientation::kWest};
  auto grid_snapshot = grid;
  std::priority_queue<Node, std::vector<Node>, CompareNode> openSet;
  std::map<Position, int> gCosts;

  int startingHeuristic = CalculateHeuristic(start, start_orientation, goal);
  Node startNode{start, start_orientation, 0, 0 + startingHeuristic, nullptr};
  openSet.push(startNode);
  gCosts[start] = 0;

  for (; !openSet.empty(); openSet.pop()) {
    Node current = openSet.top();
    if (current.position == goal) {
      return ReconstructPath(current);
    }
    for (const auto &orientation : orientations) {
      Node neighbor =
          CalculateNextState(current, orientation, grid_snapshot, goal);
      // ugly ugly ugly
      if (neighbor.position == Position::kInvalid) {
        continue;
      }

      if (gCosts.find(neighbor.position) == gCosts.end() ||
          neighbor.g_cost < gCosts[neighbor.position]) {
        gCosts[neighbor.position] = neighbor.g_cost;
        openSet.push(neighbor);
      }
    }
  }
  return std::vector<Node>();
};
// fixme: stack allocated nodes will be destroyed after A* finishes,
// cause im lazy ill copy this and fix it after we make it work with UI.
std::vector<Node> ReconstructPath(const Node &goal) {
  std::vector<Node> path;
  const Node *current = &goal;
  while (current != nullptr) {
    Node copy = *current;
    path.push_back(copy);
    current = current->parent;
  }
  std::reverse(path.begin(), path.end());
  return path;
}
Node CalculateNextState(const Node &current, const Orientation &move_direction,
                        const Grid &grid_snapshot, const Position &goal) {
  Node next = current;
  switch (move_direction) {
    case Orientation::kNorth:
      next.position.row--;
      break;
    case Orientation::kEast:
      next.position.col++;
      break;
    case Orientation::kSouth:
      next.position.row++;
      break;
    case Orientation::kWest:
      next.position.col--;
      break;
  }
  if (next.position.row < 0 || next.position.col < 0 ||
      next.position.row >= grid_snapshot.GetRows() ||
      next.position.col >= grid_snapshot.GetCols() ||
      grid_snapshot.At(next.position.row, next.position.col) ==
          CellType::kWall) {
    next.position = Position::kInvalid;
    return next;
  }
  int turnCost =
      astar::common::GetTurnCost(current.orientation, move_direction);
  int moveCost = turnCost + astar::common::ActionCost::kMoveForward;

  next.g_cost += moveCost;
  next.f_cost =
      next.g_cost + CalculateHeuristic(next.position, next.orientation, goal);
  return next;
}
// Manhattan distance, could adjust for starting orientation,
// as being on a different x/y axis guarantees having to incur
// additional cost as opposed to being aligned (not that being aligned
// guarantees being able to move forward)
int CalculateHeuristic(const Position &start,
                       const Orientation &start_orientation,
                       const Position &goal) {
  return std::abs(start.row - goal.row) + std::abs(start.col - goal.col);
}
}  // namespace astar::pathfinding
