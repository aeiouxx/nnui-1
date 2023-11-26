#include "grid.h"

#include <stdexcept>
namespace astar {
Grid::Grid(int rows, int cols)
    : rows_(rows), cols_(cols), grid_(rows * cols, CellType::kEmpty) {}
const int Grid::GetRows() const { return rows_; }
const int Grid::GetCols() const { return cols_; }
const CellType& Grid::At(int row, int col) const {
  return grid_[CalculateIndex(row, col)];
}
CellType& Grid::At(int row, int col) { return grid_[CalculateIndex(row, col)]; }

const bool Grid::IsTraversable(int row, int col) const {
  return At(row, col) != CellType::kWall;
}

const int Grid::CalculateIndex(int row, int col) const {
#ifdef ASTAR_DEBUG
  if (row < 0 || row >= rows_ || col < 0 || col >= cols_) {
    throw std::out_of_range("Index out of range");
  }
#endif
  return row * cols_ + col;
}
}  // namespace astar
