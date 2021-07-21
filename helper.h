#include <climits>
#include <cstdlib>
#include <vector>

#ifndef helper
#define helper

int grid_max(std::vector<std::vector<int>> grid, int start_x, int end_x, int start_y, int end_y);
int grid_min(std::vector<std::vector<int>> grid, int start_x, int end_x, int start_y, int end_y);
int grid_count(std::vector<std::vector<int>> grid, int start_x, int end_x, int start_y, int end_y, int element, int tolerance);

#endif