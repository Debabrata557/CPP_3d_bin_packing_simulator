#include "helper.h"

#include <climits>
#include <cstdlib>
#include <vector>
int grid_max(std::vector<std::vector<int>> grid, int start_x, int end_x, int start_y, int end_y) {
    int tmp_max = INT_MIN;
    for (int i = start_x; i < end_x; i++) {
        for (int j = start_y; j < end_y; j++) {
            tmp_max = std::max(tmp_max, grid[i][j]);
        }
    }
    return tmp_max;
}
int grid_min(std::vector<std::vector<int>> grid, int start_x, int end_x, int start_y, int end_y) {
    int tmp_min = INT_MAX;
    for (int i = start_x; i < end_x; i++) {
        for (int j = start_y; j < end_y; j++) {
            tmp_min = std::min(tmp_min, grid[i][j]);
        }
    }
    return tmp_min;
}

std::pair<int, int> grid_max_min(std::vector<std::vector<int>> grid, int start_x, int end_x, int start_y, int end_y) {
    int tmp_min = INT_MAX;
    int tmp_max = INT_MIN;
    for (int i = start_x; i < end_x; i++) {
        for (int j = start_y; j < end_y; j++) {
            tmp_min = std::min(tmp_min, grid[i][j]);
            tmp_max = std::max(tmp_min, grid[i][j]);
        }
    }
    return {tmp_max, tmp_min};
}

int grid_count(std::vector<std::vector<int>> grid, int start_x, int end_x, int start_y, int end_y, int element, int tolerance) {
    int count = 0;
    for (int i = start_x; i < end_x; i++) {
        for (int j = start_y; j < end_y; j++) {
            count += (abs(grid[i][j] - element) <= tolerance);
        }
    }
    return count;
}