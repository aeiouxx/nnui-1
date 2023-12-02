#include "grid.h"

#include <stdexcept>
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

Grid::Grid(int rows, int cols)
    : rows_(rows), cols_(cols), grid_(rows * cols, CellType::kEmpty) {
}
Grid::Grid(const Grid& other)
    : rows_(other.rows_), cols_(other.cols_), grid_(other.grid_) {
}
Grid::Grid(Grid&& other) noexcept
    : rows_(other.rows_), cols_(other.cols_), grid_(std::move(other.grid_)) {
  other.rows_ = 0;
  other.cols_ = 0;
}
Grid& Grid::operator=(const Grid& other) {
  rows_ = other.rows_;
  cols_ = other.cols_;
  grid_ = other.grid_;
  return *this;
}
Grid& Grid::operator=(Grid&& other) noexcept {
  rows_ = other.rows_;
  cols_ = other.cols_;
  grid_ = std::move(other.grid_);
  other.rows_ = 0;
  other.cols_ = 0;
  return *this;
}
const int Grid::GetRows() const {
  return rows_;
}
const int Grid::GetCols() const {
  return cols_;
}
const CellType& Grid::At(int row, int col) const {
  return grid_[CalculateIndex(row, col)];
}
CellType& Grid::At(int row, int col) {
  return grid_[CalculateIndex(row, col)];
}

const bool Grid::IsEmpty() const {
  return !(rows_ | cols_);
}
const bool Grid::IsTraversable(int row, int col) const {
  if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
    return false;
  }
  return At(row, col) != CellType::kWall;
}

void Grid::ToggleStart(int row, int col) {
#ifdef ASTAR_DEBUG
  if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
    throw std::out_of_range("Index out of range");
  }
#endif
  if (start_.IsValid()) {
    At(start_.row, start_.col) = CellType::kEmpty;
  }
  if (start_ == Position(row, col)) {
    start_ = Position(-1, -1);
  } else {
    start_ = Position(row, col);
    At(row, col) = CellType::kStart;
  }
}
Position Grid::GetStart() const {
  return start_;
}
void Grid::ToggleGoal(int row, int col) {
#ifdef ASTAR_DEBUG
  if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
    throw std::out_of_range("Index out of range");
  }
#endif
  if (goal_.IsValid()) {
    At(goal_.row, goal_.col) = CellType::kEmpty;
  }
  if (goal_ == Position(row, col)) {
    goal_ = Position(-1, -1);
  } else {
    goal_ = Position(row, col);
    At(row, col) = CellType::kGoal;
  }
}
Position Grid::GetGoal() const {
  return goal_;
}
const int Grid::CalculateIndex(int row, int col) const {
#ifdef ASTAR_DEBUG
  if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
    throw std::out_of_range("Index out of range");
  }
#endif
  return row * cols_ + col;
}
}  // namespace astar::common
