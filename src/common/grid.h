#ifndef ASTAR_COMMON_GRID_H
#define ASTAR_COMMON_GRID_H
#include <vector>

#include "definitions.h"
namespace astar {
class Grid {
 public:
  Grid(int rows, int cols);
  const int GetRows() const;
  const int GetCols() const;

  const CellType& At(int row, int col) const;
  CellType& At(int row, int col);

 private:
  int CalculateIndex(int row, int col) const;

 private:
  std::vector<CellType> grid_;
  int rows_;
  int cols_;
};
}  // namespace astar
#endif
