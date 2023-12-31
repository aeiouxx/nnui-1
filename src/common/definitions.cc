#include "definitions.h"

namespace astar::common {
const Position Position::kInvalid{-1, -1};
Position::Position(int row, int col) : row(row), col(col) {
}
bool Position::IsValid() const {
  return row >= 0 && col >= 0;
}
bool Position::operator==(const Position &other) const {
  return row == other.row && col == other.col;
}
bool Position::operator!=(const Position &other) const {
  return !(*this == other);
}

bool Position::operator<(const Position &other) const {
  if (row == other.row) {
    return col < other.col;
  }
  return row < other.row;
}
int GetTurnCost(const Orientation &current, const Orientation &goal) {
  auto magic = current ^ goal;
  if (!magic) {
    return 0;
  }
  return (magic % 2) ? ActionCost::kTurn90 : ActionCost::kTurn180;
}
};  // namespace astar::common
