#include "config.h"

bool operator<(const vector_3d& a, const vector_3d& b) {
    if (a.x != b.x) {
        return a.x < b.x;
    } else {
        if (a.y != b.y) {
            return a.y < b.y;
        } else {
            return a.z < b.z;
        }
    }
}

bool comp_floor_building(std::pair<vector_3d, vector_3d>& a, std::pair<vector_3d, vector_3d>& b) {
    std::pair<vector_3d, vector_3d> temp_a = {{a.first.z, a.first.x, a.first.y}, {a.second.z, a.second.x, a.second.y}};
    std::pair<vector_3d, vector_3d> temp_b = {{b.first.z, b.first.x, b.first.y}, {b.second.z, b.second.x, b.second.y}};
    return temp_a < temp_b;
}