#include "algorithm.h"

#include <map>
#include <queue>

#include "../ui/maze-update.h"
#include "wx/log.h"

namespace astar::pathfinding {
using astar::common::CellType;
using astar::common::Grid;
using astar::common::Orientation;
using astar::common::Position;
using astar::ui::MazeUpdate;

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

void AstarAlgorithm::Run(const Grid &grid, const Position &start,
                         const Orientation &start_orientation,
                         const Position &goal, wxEvtHandler *update_target) {
  if (is_running.exchange(true)) {
    wxLogError("Run called while algorithm is already running");
    return;
  }
  if (working_copy != nullptr || !owned_nodes.empty()) {
    wxLogError("Something is wrong with the resource management:\nrunning: %s",
               is_running ? "true" : "false");
    Cleanup();
  }
  working_copy = new Grid(grid);
  std::priority_queue<Node *, std::vector<Node *>, CompareNode> openSet;
  std::map<Position, int> gCosts;
  std::vector<ui::MazeUpdate> updatesForUi;
  int startingHeuristic = CalculateHeuristic(start, start_orientation, goal);
  Node *startNode = CreateOwnedNode(start, start_orientation, 0,
                                    0 + startingHeuristic, nullptr);
  openSet.push(startNode);
  gCosts[startNode->position] = 0;

  unsigned int checks = 0;
  ui::MazeUpdateEvent updateEvent;
  for (; !openSet.empty() && !cancellation_requested; openSet.pop()) {
    auto *current = openSet.top();
    updatesForUi.push_back({current->position, CellType::kVisited});
    wxLogDebug("Current(f:%d) %d,%d", current->f_cost, current->position.row,
               current->position.col);
    wxLogDebug("Current_parent(f:%d) %d,%d", current->f_cost,
               current->position.row, current->position.col);
    if (current->position == goal) {
      wxLogDebug("Found goal in %d checks", checks);
      EmitPath(current, update_target);
      updatesForUi.clear();
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
    if (++checks % checks_per_update == 0) {
      updateEvent.SetUpdates(updatesForUi);
      wxQueueEvent(update_target, updateEvent.Clone());
      updatesForUi.clear();
    }
  }

  if (updatesForUi.size() > 0) {
    updateEvent.SetUpdates(updatesForUi);
    wxQueueEvent(update_target, updateEvent.Clone());
    updatesForUi.clear();
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
  if (!working_copy->IsTraversable(newPosition.row, newPosition.col)) {
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
  auto heuristic =
      std::abs(position.row - goal.row) + std::abs(position.col - goal.col);
  return heuristic;
}
void AstarAlgorithm::EmitPath(Node *goal, wxEvtHandler *update_target) {
  ui::MazeUpdateEvent updateEvent;
  std::vector<MazeUpdate> path;
  for (auto *current = goal; current != nullptr; current = current->parent) {
    path.push_back({current->position, CellType::kPath});
  }
  updateEvent.SetUpdates(path);
  // clone might be problematic, because we move the vector (should probably
  // copy) but let's let it crash first.
  wxQueueEvent(update_target, updateEvent.Clone());
  Cleanup();
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
  is_running = false;
}
}  // namespace astar::pathfinding
