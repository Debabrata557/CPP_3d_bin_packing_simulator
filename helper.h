#include <climits>
#include <cstdlib>
#include <vector>

#ifndef helper
#define helper

int grid_max(const std::vector<std::vector<int>> &grid, int start_x, int end_x, int start_y, int end_y);
int grid_min(const std::vector<std::vector<int>> &grid, int start_x, int end_x, int start_y, int end_y);
int grid_count(const std::vector<std::vector<int>> &grid, int start_x, int end_x, int start_y, int end_y, int element, int tolerance);
std::pair<int, int> grid_max_min(const std::vector<std::vector<int>> &grid, int start_x, int end_x, int start_y, int end_y);
int factorial(int n);
#endif