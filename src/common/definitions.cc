#include "definitions.h"

namespace astar::common {
Position::Position(int row, int col) : row(row), col(col) {
}
bool Position::IsValid() const {
  return row >= 0 && col >= 0;
}
bool Position::operator==(const Position& other) const {
  return row == other.row && col == other.col;
}
bool Position::operator!=(const Position& other) const {
  return !(*this == other);
}
};  // namespace astar::common
