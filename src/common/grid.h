#ifndef ASTAR_COMMON_GRID_H
#define ASTAR_COMMON_GRID_H
#include <vector>

#include "definitions.h"
namespace astar::common {
class Grid {
 public:
  Grid(int rows, int cols);
  // Could be default
  Grid(const Grid &other);
  Grid &operator=(const Grid &other);
  Grid(Grid &&other) noexcept;
  Grid &operator=(Grid &&other) noexcept;
  const int GetRows() const;
  const int GetCols() const;
  const CellType &At(int row, int col) const;
  CellType &At(int row, int col);
  const bool IsEmpty() const;
  // Anything that is not within the grid in considered not traversable
  const bool IsTraversable(int row, int col) const;
  void ToggleStart(int row, int col);
  Position GetStart() const;
  Position GetGoal() const;
  void ToggleGoal(int row, int col);

  void ClearPathInfo();

 private:
  const int CalculateIndex(int row, int col) const;

 private:
  std::vector<CellType> grid_;
  int rows_;
  int cols_;

  // Start / goal coordinates
 private:
  Position start_{-1, -1};
  Position goal_{-1, -1};
};
}  // namespace astar::common
#endif
