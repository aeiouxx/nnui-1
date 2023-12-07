#include "algorithm.h"

#include <map>
#include <queue>

#include "wx/log.h"

namespace astar::pathfinding {
using astar::common::CellType;
bool CompareNode::operator()(const Node *lhs, const Node *rhs) const {
  return lhs->f_cost > rhs->f_cost;
}
const Orientation AstarAlgorithm::kOrientations[] = {
    Orientation::kNorth, Orientation::kEast, Orientation::kSouth,
    Orientation::kWest};
AstarAlgorithm::~AstarAlgorithm() {
  Cleanup();
}
void AstarAlgorithm::RequestCancellation() {
  cancellation_requested = true;
}
// updates are sent to the grid as they are calculated,
// node ownership remains within the algorithm.

// issue of thread safety when it comes to the grid,
// user can reposition start / goal while the algorithm is running,

// could just create a working copy of the grid.
void AstarAlgorithm::Run(const Grid &grid, const Position &start,
                         const Orientation &start_orientation,
                         const Position &goal) {
  if (working_copy != nullptr) {
    wxLogDebug("Working copy already exists, cleaning up");
    Cleanup();
  }
  working_copy = new Grid(grid);
  std::priority_queue<Node *, std::vector<Node *>, CompareNode> openSet;
  std::map<Position, int> gCosts;
  int startingHeuristic = CalculateHeuristic(start, start_orientation, goal);
  Node *startNode = CreateOwnedNode(start, start_orientation, 0,
                                    0 + startingHeuristic, nullptr);
  openSet.push(startNode);
  gCosts[startNode->position] = 0;

  for (; !openSet.empty() && !cancellation_requested; openSet.pop()) {
    auto *current = openSet.top();
    if (current->position == goal) {
      wxLogInfo("Found path");
      SendPath(current);
      break;
    }
    for (const auto &orientation : kOrientations) {
      if (cancellation_requested) {
        break;
      }
      auto *neighbor = CalculateNeighbor(current, orientation, goal);
      if (neighbor == nullptr) {
        continue;
      }

      if (gCosts.find(neighbor->position) == gCosts.end() ||
          neighbor->g_cost < gCosts[neighbor->position]) {
        gCosts[neighbor->position] = neighbor->g_cost;
        openSet.push(neighbor);
      }
    }
  }
  Cleanup();
}
Node *AstarAlgorithm::CreateOwnedNode(const Position &position,
                                      const Orientation &orientation,
                                      int g_cost, int f_cost, Node *parent) {
  Node *node = new Node{position, orientation, g_cost, f_cost, parent};
  owned_nodes.push_back(node);
  return node;
}
Node *AstarAlgorithm::CalculateNeighbor(Node *current,
                                        const Orientation &move_direction,
                                        const Position &goal) {
  auto newPosition = current->position;
  switch (move_direction) {
    case Orientation::kNorth:
      newPosition.row--;
      break;
    case Orientation::kEast:
      newPosition.col++;
      break;
    case Orientation::kSouth:
      newPosition.row++;
      break;
    case Orientation::kWest:
      newPosition.col--;
      break;
  }
  if (newPosition.row < 0 || newPosition.col < 0 ||
      newPosition.row >= working_copy->GetRows() ||
      newPosition.col >= working_copy->GetCols() ||
      working_copy->At(newPosition.row, newPosition.col) == CellType::kWall) {
    return nullptr;
  }
  Node *neighbor = CreateOwnedNode(newPosition, move_direction, current->g_cost,
                                   current->f_cost, current);
  int turnCost =
      astar::common::GetTurnCost(current->orientation, move_direction);
  int moveCost = turnCost + astar::common::ActionCost::kMoveForward;
  neighbor->g_cost += moveCost;
  neighbor->f_cost =
      neighbor->g_cost +
      CalculateHeuristic(neighbor->position, neighbor->orientation, goal);
  return neighbor;
}
// for now just uses manhattan distance, should be modified
int AstarAlgorithm::CalculateHeuristic(const Position &position,
                                       const Orientation &move_direction,
                                       const Position &goal) {
  return std::abs(position.row - goal.row) + std::abs(position.col - goal.col);
}
void AstarAlgorithm::SendPath(Node *goal) {
}
void AstarAlgorithm::Cleanup() {
  for (auto *node : owned_nodes) {
    delete node;
  }
  owned_nodes.clear();
  if (working_copy != nullptr) {
    delete working_copy;
    working_copy = nullptr;
  }
}
}  // namespace astar::pathfinding
