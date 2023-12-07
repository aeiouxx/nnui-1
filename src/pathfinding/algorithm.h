#ifndef ASTAR_PATHFINDING_ALGORITHM_H
#define ASTAR_PATHFINDING_ALGORITHM_H
#include <atomic>
#include <limits>
#include <vector>

#include "../common/definitions.h"
#include "../common/grid.h"
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
  bool operator()(const Node *lhs, const Node *rhs) const;
};
// can own the A* nodes as they get transferred to the grid only in the form of
// updates, ownership of the nodes remains within the algorithm.

// will need to cleanup the nodes on cancellation request / finishing the
// search.
struct AstarAlgorithm {
  // this is ugly and shouldn't be done like this
  // but creating an iterator for the enum is a PITA
  static const Orientation kOrientations[];
  std::atomic_bool cancellation_requested = false;
  std::vector<Node *> owned_nodes;
  Grid *working_copy = nullptr;

  ~AstarAlgorithm();
  void RequestCancellation();
  void Run(const Grid &grid, const Position &start,
           const Orientation &start_orientation, const Position &goal);

 private:
  Node *CreateOwnedNode(const Position &position,
                        const Orientation &orientation, int g_cost, int f_cost,
                        Node *parent);
  Node *CalculateNeighbor(Node *current, const Orientation &move_direction,
                          const Position &goal);
  int CalculateHeuristic(const Position &start,
                         const Orientation &start_orientation,
                         const Position &goal);
  void SendPath(Node *goal);
  void Cleanup();
};
}  // namespace astar::pathfinding
#endif
