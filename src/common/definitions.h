#ifndef ASTAR_COMMON_DEFINITIONS_H
#define ASTAR_COMMON_DEFINITIONS_H
namespace astar {
#define ASTAR_NOCOPY_NOMOVE(Class)         \
  Class(const Class&) = delete;            \
  Class(Class&&) = delete;                 \
  Class& operator=(const Class&) = delete; \
  Class& operator=(Class&&) = delete;

enum class CellType {
  kEmpty = 0,
  kStart,
  kPath,
  kGoal,
  kVisited,
  kWall,
};
enum class Orientation {
  kNorth,
  kEast,
  kSouth,
  kWest
};

enum ActionCost {
  kMoveForward = 5,
  kTurn90 = 2,
  kTurn180 = 3
};
}  // namespace astar
#endif
