#ifndef ASTAR_COMMON_DEFINITIONS_H
#define ASTAR_COMMON_DEFINITIONS_H
namespace astar::common {
enum class CellType {
  kEmpty = 0,
  kStart,
  kPath,
  kGoal,
  kVisited,
  kWall,
};
// same axis has the same parity
enum Orientation {
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
struct Position {
  static const Position kInvalid;
  int row;
  int col;

  Position(int row = -1, int col = -1);
  bool IsValid() const;
  bool operator==(const Position &other) const;
  bool operator!=(const Position &other) const;

  bool operator<(const Position &other) const;
};
int GetTurnCost(const Orientation &current, const Orientation &goal);
}  // namespace astar::common
#endif
