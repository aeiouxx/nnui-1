#ifndef ASTAR_PATHFINDING_ALGORITHM_H
#define ASTAR_PATHFINDING_ALGORITHM_H
#include <atomic>
#include <limits>
#include <vector>

#include "../common/definitions.h"
#include "../common/grid.h"
#include "wx/event.h"
namespace astar::pathfinding {
struct Node {
  astar::common::Position position = astar::common::Position::kInvalid;
  astar::common::Orientation orientation = astar::common::Orientation::kNorth;
  int g_cost = std::numeric_limits<int>::max();
  int f_cost = std::numeric_limits<int>::max();

  Node *parent = nullptr;
};
struct CompareNode {
  bool operator()(const Node *lhs, const Node *rhs) const;
};
struct AstarAlgorithm {
  // because i'm too lazy to create an iterator for the enum, definitely
  // shouldn't be done like this.
  static const astar::common::Orientation kOrientations[];
  std::atomic_bool cancellation_requested = false;
  std::atomic_bool is_running = false;
  unsigned int checks_per_update = 16;

  ~AstarAlgorithm();
  void RequestCancellation();
  // Path will be posted to the UI via a custom wxEvent.
  void Run(const common::Grid &grid, const common::Position &start,
           const common::Orientation &start_orientation,
           const common::Position &goal, wxEvtHandler *update_target = nullptr);

 private:
  Node *CreateOwnedNode(const common::Position &position,
                        const common::Orientation &orientation, int g_cost,
                        int f_cost, Node *parent);
  Node *CalculateNeighbor(Node *current,
                          const common::Orientation &move_direction,
                          const common::Position &goal);
  int CalculateHeuristic(const common::Position &start,
                         const common::Orientation &start_orientation,
                         const common::Position &goal);
  void EmitPath(Node *goal, wxEvtHandler *update_target);
  void Cleanup();

 private:
  std::vector<Node *> owned_nodes;
  common::Grid *working_copy = nullptr;
};
}  // namespace astar::pathfinding
#endif
