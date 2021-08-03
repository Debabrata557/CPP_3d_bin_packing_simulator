#include "base.h"

std::vector<vector_3d> boxes;
std::vector<std::vector<std::vector<std::vector<int>>>> pre_computed_max;
std::vector<std::vector<std::vector<std::vector<int>>>> pre_computed_min;
std::pair<int, int> Base::compute_max_min(int x1, int x2, int y1, int y2) {
    //std::cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<" "<<std::endl;
    int k = log2(x2 - x1 + 1);
    int l = log2(y2 - y1 + 1);
    //std::cout << "Value of k="<<k<<" l="<<l<<std::endl;
    int ans_max = std::max({pre_computed_max[x1][y1][k][l], pre_computed_max[x2 - (1 << k) + 1][y1][k][l], pre_computed_max[x1][y2 - (1 << l) + 1][k][l], pre_computed_max[x2 - (1 << k) + 1][y2 - (1 << l) + 1][k][l]});
    int ans_min = std::min({pre_computed_min[x1][y1][k][l], pre_computed_min[x2 - (1 << k) + 1][y1][k][l], pre_computed_min[x1][y2 - (1 << l) + 1][k][l], pre_computed_min[x2 - (1 << k) + 1][y2 - (1 << l) + 1][k][l]});
    return {ans_max, ans_min};
}
void Base::precompute_max_min(const std::vector<std::vector<int>> &state) {
    for (int i = 0; (1 << i) <= MAX_BOX_WIDTH; i += 1) {
        for (int j = 0; (1 << j) <= MAX_BOX_LENGTH; j += 1) {
            for (int x = 0; x + (1 << i) - 1 < BIN_WIDTH; x += 1) {
                for (int y = 0; y + (1 << j) - 1 < BIN_LENGTH; y += 1) {
                    if (i == 0 and j == 0) {
                        pre_computed_max[x][y][i][j] = state[x][y];  // store x, y
                        pre_computed_min[x][y][i][j] = state[x][y];
                    } else if (i == 0) {
                        pre_computed_max[x][y][i][j] = std::max(pre_computed_max[x][y][i][j - 1], pre_computed_max[x][y + (1 << (j - 1))][i][j - 1]);
                        pre_computed_min[x][y][i][j] = std::min(pre_computed_min[x][y][i][j - 1], pre_computed_min[x][y + (1 << (j - 1))][i][j - 1]);
                    } else if (j == 0) {
                        pre_computed_max[x][y][i][j] = std::max(pre_computed_max[x][y][i - 1][j], pre_computed_max[x + (1 << (i - 1))][y][i - 1][j]);
                        pre_computed_min[x][y][i][j] = std::min(pre_computed_min[x][y][i - 1][j], pre_computed_min[x + (1 << (i - 1))][y][i - 1][j]);
                    } else {
                        pre_computed_max[x][y][i][j] = std::max({pre_computed_max[x][y][i - 1][j - 1], pre_computed_max[x + (1 << (i - 1))][y][i - 1][j - 1], pre_computed_max[x][y + (1 << (j - 1))][i - 1][j - 1], pre_computed_max[x + (1 << (i - 1))][y + (1 << (j - 1))][i - 1][j - 1]});
                        pre_computed_min[x][y][i][j] = std::min({pre_computed_min[x][y][i - 1][j - 1], pre_computed_min[x + (1 << (i - 1))][y][i - 1][j - 1], pre_computed_min[x][y + (1 << (j - 1))][i - 1][j - 1], pre_computed_min[x + (1 << (i - 1))][y + (1 << (j - 1))][i - 1][j - 1]});
                    }
                    // cout << "from i="<<x<<" j="<<y<<" of length="<<(1<<i)<<" and length="<<(1<<j) <<"max is: " << M[x][y][i][j] << endl;
                }
            }
        }
    }
}
Base::Base() {
}
Base::Base(GenerateBox gb, std::vector<Bin> bin_instances) {
    this->gb = gb;
    this->bin_instances = bin_instances;
    boxes = gb.get_stream_of_boxes();
}
Base::~Base() {
}

bool Base::check_with_precomputation(const std::vector<std::vector<int>> &state, std::pair<int, int> pos, vector_3d dim) {
    int lx = dim.x;
    int ly = dim.y;
    int lz = dim.z;

    int minx = pos.first;
    int maxx = pos.first + lx - 1;
    int miny = pos.second;
    int maxy = pos.second + ly - 1;
    int surface = 5;

    if (minx >= 0 && miny >= 0 && maxx <= BIN_WIDTH - 1 && maxy <= BIN_LENGTH - 1) {
        // int max_height = grid_max(state, minx, maxx + 1, miny, maxy + 1);
        // std::pair<int, int> corner1_max_min = grid_max_min(state, minx, minx + surface + 1, miny, miny + surface + 1);
        // std::pair<int, int> corner2_max_min = grid_max_min(state, maxx - surface, maxx + 1, miny, miny + surface + 1);
        // std::pair<int, int> corner3_max_min = grid_max_min(state, minx, minx + surface + 1, maxy, maxy - surface + 1);
        // std::pair<int, int> corner4_max_min = grid_max_min(state, maxx - surface, maxx + 1, maxy - surface, maxy + 1);
        // int corner1_max = corner1_max_min.first;
        // int corner2_max = corner2_max_min.first;
        // int corner3_max = corner3_max_min.first;
        // int corner4_max = corner4_max_min.first;
        // int corner1_min = corner1_max_min.second;
        // int corner2_min = corner2_max_min.second;
        // int corner3_min = corner3_max_min.second;
        // int corner4_min = corner4_max_min.second;
        int max_height = compute_max_min(minx, maxx, miny, maxy).first;
        std::pair<int, int> corner1_max_min = compute_max_min(minx, minx + surface, miny, miny + surface);
        std::pair<int, int> corner2_max_min = compute_max_min(maxx - surface, maxx, miny, miny + surface);
        std::pair<int, int> corner3_max_min = compute_max_min(minx, minx + surface, maxy - surface, maxy);
        std::pair<int, int> corner4_max_min = compute_max_min(maxx - surface, maxx, maxy - surface, maxy);
        int corner1_max = corner1_max_min.first;
        int corner2_max = corner2_max_min.first;
        int corner3_max = corner3_max_min.first;
        int corner4_max = corner4_max_min.first;
        int corner1_min = corner1_max_min.second;
        int corner2_min = corner2_max_min.second;
        int corner3_min = corner3_max_min.second;
        int corner4_min = corner4_max_min.second;

        if (max_height + lz >= BIN_HEIGHT) {
            return 0;
        }
        double support = grid_count(state, minx, maxx + 1, miny, maxy + 1, max_height, CONTROLLER_TOLERANCE) / (double)(lx * ly);
        int corner_support = 0;
        corner_support += (abs(corner1_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner1_min - max_height) <= CONTROLLER_TOLERANCE);
        corner_support += (abs(corner2_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner2_min - max_height) <= CONTROLLER_TOLERANCE);
        corner_support += (abs(corner3_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner3_min - max_height) <= CONTROLLER_TOLERANCE);
        corner_support += (abs(corner4_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner4_min - max_height) <= CONTROLLER_TOLERANCE);
        if (corner_support == 4)
            return support >= 0.6;
        if (corner_support == 3)
            return support >= 0.8;
        return support >= 0.95;
    }
    return 0;
}

bool Base::check_without_precomputation(const std::vector<std::vector<int>> &state, std::pair<int, int> pos, vector_3d dim) {
    int lx = dim.x;
    int ly = dim.y;
    int lz = dim.z;

    int minx = pos.first;
    int maxx = pos.first + lx - 1;
    int miny = pos.second;
    int maxy = pos.second + ly - 1;
    int surface = 5;

    if (minx >= 0 && miny >= 0 && maxx <= BIN_WIDTH - 1 && maxy <= BIN_LENGTH - 1) {
        int max_height = grid_max(state, minx, maxx + 1, miny, maxy + 1);
        std::pair<int, int> corner1_max_min = grid_max_min(state, minx, minx + surface + 1, miny, miny + surface + 1);
        std::pair<int, int> corner2_max_min = grid_max_min(state, maxx - surface, maxx + 1, miny, miny + surface + 1);
        std::pair<int, int> corner3_max_min = grid_max_min(state, minx, minx + surface + 1, maxy, maxy - surface + 1);
        std::pair<int, int> corner4_max_min = grid_max_min(state, maxx - surface, maxx + 1, maxy - surface, maxy + 1);
        int corner1_max = corner1_max_min.first;
        int corner2_max = corner2_max_min.first;
        int corner3_max = corner3_max_min.first;
        int corner4_max = corner4_max_min.first;
        int corner1_min = corner1_max_min.second;
        int corner2_min = corner2_max_min.second;
        int corner3_min = corner3_max_min.second;
        int corner4_min = corner4_max_min.second;
        // int max_height = compute_max_min(minx, maxx, miny, maxy).first;
        // std::pair<int, int> corner1_max_min = compute_max_min(minx, minx + surface, miny, miny + surface);
        // std::pair<int, int> corner2_max_min = compute_max_min(maxx - surface, maxx, miny, miny + surface);
        // std::pair<int, int> corner3_max_min = compute_max_min(minx, minx + surface, maxy - surface, maxy);
        // std::pair<int, int> corner4_max_min = compute_max_min(maxx - surface, maxx, maxy - surface, maxy);
        // int corner1_max = corner1_max_min.first;
        // int corner2_max = corner2_max_min.first;
        // int corner3_max = corner3_max_min.first;
        // int corner4_max = corner4_max_min.first;
        // int corner1_min = corner1_max_min.second;
        // int corner2_min = corner2_max_min.second;
        // int corner3_min = corner3_max_min.second;
        // int corner4_min = corner4_max_min.second;

        if (max_height + lz >= BIN_HEIGHT) {
            return 0;
        }
        double support = grid_count(state, minx, maxx + 1, miny, maxy + 1, max_height, CONTROLLER_TOLERANCE) / (double)(lx * ly);
        int corner_support = 0;
        corner_support += (abs(corner1_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner1_min - max_height) <= CONTROLLER_TOLERANCE);
        corner_support += (abs(corner2_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner2_min - max_height) <= CONTROLLER_TOLERANCE);
        corner_support += (abs(corner3_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner3_min - max_height) <= CONTROLLER_TOLERANCE);
        corner_support += (abs(corner4_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner4_min - max_height) <= CONTROLLER_TOLERANCE);
        if (corner_support == 4)
            return support >= 0.6;
        if (corner_support == 3)
            return support >= 0.8;
        return support >= 0.95;
    }
    return 0;
}