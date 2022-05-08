#include "base.h"

std::vector<vector_3d> boxes;
std::vector<std::vector<std::vector<std::vector<int>>>> pre_computed_max;
std::vector<std::vector<std::vector<std::vector<int>>>> pre_computed_min;

std::pair<int, int> Base::compute_max_min(int x1, int x2, int y1, int y2) {
    // std::cout<<x1<<" "<<y1<<" "<<x2<<" "<<y2<<" "<<std::endl;
    int k = log2(x2 - x1 + 1);
    int l = log2(y2 - y1 + 1);
    // std::cout << "Value of k="<<k<<" l="<<l<<std::endl;
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
Base::Base(GenerateBox gb, Sim &simulator) {
    pre_computed_max = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(MAX_BOX_WIDTH + 1, std::vector<int>(MAX_BOX_LENGTH + 1))));
    pre_computed_min = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(MAX_BOX_WIDTH + 1, std::vector<int>(MAX_BOX_LENGTH + 1))));
    this->gb = gb;
    // this->simulator = simulator;
    boxes = gb.get_stream_of_boxes();
}
Base::~Base() {
}

bool Base::check_with_precomputation(const std::vector<std::vector<int>> &state, vector_3d pos, vector_3d dim) {
    int lx = dim.x + 4;
    int ly = dim.y + 4;
    int lz = dim.z;

    int minx = pos.x;
    int maxx = pos.x + lx - 1;
    int miny = pos.y;
    int maxy = pos.y + ly - 1;
    if (pos.z == 1) {
        miny = pos.y - ly + 1;
        maxy = pos.y;
    } else if (pos.z == 2) {
        minx = pos.x - lx + 1;
        maxx = pos.x;
    } else if (pos.z == 3) {
        miny = pos.y - ly + 1;
        maxy = pos.y;
        minx = pos.x - lx + 1;
        maxx = pos.x;
    }
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

        if (max_height + lz > BIN_HEIGHT) {
            return 0;
        }
        double support = grid_count(state, minx, maxx + 1, miny, maxy + 1, max_height, CONTROLLER_TOLERANCE) / (double)(lx * ly);
        int corner_support = 0;
        corner_support += (grid_count(state, minx, minx + surface + 1, miny, miny + surface + 1, corner1_max, CONTROLLER_TOLERANCE) / (double)((surface + 1) * (surface + 1))) >= 0.85;
        corner_support += (grid_count(state, maxx - surface, maxx + 1, miny, miny + surface + 1, corner2_max, CONTROLLER_TOLERANCE) / (double)((surface + 1) * (surface + 1))) >= 0.85;
        corner_support += (grid_count(state, minx, minx + surface + 1, maxy, maxy - surface + 1, corner3_max, CONTROLLER_TOLERANCE) / (double)((surface + 1) * (surface + 1))) >= 0.85;
        corner_support += (grid_count(state, maxx - surface, maxx + 1, maxy - surface, maxy + 1, corner4_max, CONTROLLER_TOLERANCE) / (double)((surface + 1) * (surface + 1))) >= 0.85;
        if (corner_support == 4)
            return support >= 0.7;
        if (corner_support == 3 || corner_support == 0)
            return support >= 0.9;
        return support >= 0.98;
    }
    return 0;
}

bool Base::check_without_precomputation(const std::vector<std::vector<int>> &state, vector_3d pos, vector_3d dim) {
    int lx = dim.x + 4;
    int ly = dim.y + 4;
    int lz = dim.z;

    int minx = pos.x;
    int maxx = pos.x + lx - 1;
    int miny = pos.y;
    int maxy = pos.y + ly - 1;
    if (pos.z == 1) {
        miny = pos.y - ly + 1;
        maxy = pos.y;
    } else if (pos.z == 2) {
        minx = pos.x - lx + 1;
        maxx = pos.x;
    } else if (pos.z == 3) {
        miny = pos.y - ly + 1;
        maxy = pos.y;
        minx = pos.x - lx + 1;
        maxx = pos.x;
    }
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

        if (max_height + lz > BIN_HEIGHT) {
            return 0;
        }
        double support = grid_count(state, minx, maxx + 1, miny, maxy + 1, max_height, CONTROLLER_TOLERANCE) / (double)(lx * ly);
        int corner_support = 0;
        corner_support += (grid_count(state, minx, minx + surface + 1, miny, miny + surface + 1, corner1_max, CONTROLLER_TOLERANCE) / (double)((surface + 1) * (surface + 1))) >= 0.85;
        corner_support += (grid_count(state, maxx - surface, maxx + 1, miny, miny + surface + 1, corner2_max, CONTROLLER_TOLERANCE) / (double)((surface + 1) * (surface + 1))) >= 0.85;
        corner_support += (grid_count(state, minx, minx + surface + 1, maxy, maxy - surface + 1, corner3_max, CONTROLLER_TOLERANCE) / (double)((surface + 1) * (surface + 1))) >= 0.85;
        corner_support += (grid_count(state, maxx - surface, maxx + 1, maxy - surface, maxy + 1, corner4_max, CONTROLLER_TOLERANCE) / (double)((surface + 1) * (surface + 1))) >= 0.85;
        if (corner_support == 4)
            return support >= 0.7;
        if (corner_support == 3 || corner_support == 0)
            return support >= 0.9;
        return support >= 0.98;
    }
    return 0;
}