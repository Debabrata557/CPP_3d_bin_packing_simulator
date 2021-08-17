#include "bin.h"

#include <set>

Bin::Bin(/* args */) {
    bin_state = std::vector<std::vector<int>>(BIN_WIDTH, std::vector<int>(BIN_LENGTH, 0));
    icpbcp_list = std::vector<std::pair<vector_3d, vector_3d>>({{{0, 0, 0}, {BIN_WIDTH, BIN_LENGTH, BIN_HEIGHT}}});
    open = true;
    volume = 0;
    no_of_boxes_placed = 0;
}
Bin::~Bin() {
}
bool Bin::is_open() {
    return open;
}
int Bin::update_state(std::pair<int, int> start_corner, vector_3d dim) {
    int width = dim.x;
    int length = dim.y;
    int height = dim.z;
    int cur_max_height = -1;
    int scaled_length = length * SCALING_FACTOR;
    int scaled_width = width * SCALING_FACTOR;
    int scaled_height = height * SCALING_FACTOR;
    try {
        for (int start_x = start_corner.first; start_x <= start_corner.first + scaled_width - 1; start_x++) {
            for (int start_y = start_corner.second; start_y <= start_corner.second + scaled_length - 1; start_y++) {
                bin_state[start_x][start_y] += scaled_height;
                cur_max_height = std::max(cur_max_height, bin_state[start_x][start_y]);
            }
        }
        for (int start_x = start_corner.first; start_x <= start_corner.first + scaled_width - 1; start_x++) {
            for (int start_y = start_corner.second; start_y <= start_corner.second + scaled_length - 1; start_y++) {
                bin_state[start_x][start_y] = cur_max_height;
            }
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 0;
    }

    return 1;
}

bool Bin::is_overlapping(std::pair<vector_3d, vector_3d> icpbcp, vector_3d pos, vector_3d size) {
    for (int i = 0; i <= 1; i++) {
        for (int j = 0; j <= 1; j++) {
            for (int k = 0; k <= 1; k++) {
                int cur_x = pos.x + i * size.x;
                int cur_y = pos.y + j * size.y;
                int cur_z = pos.z + k * size.z;
                if (cur_x > icpbcp.first.x && cur_x < icpbcp.second.x && cur_y > icpbcp.first.y && cur_y < icpbcp.second.y && cur_z > icpbcp.first.z && cur_z < icpbcp.second.z) {
                    return true;
                }
            }
        }
    }
    return false;
}

void Bin::set_open() {
    std::vector<std::pair<vector_3d, vector_3d>> temp_icpbcp_list;
    bool flag = false;
    for (auto icp_bcp : icpbcp_list) {
        int x_diff = icp_bcp.second.x - icp_bcp.first.x;
        int y_diff = icp_bcp.second.y - icp_bcp.first.y;
        int z_diff = icp_bcp.second.z - icp_bcp.first.z;

        if (x_diff >= MIN_BOX_WIDTH && y_diff >= MIN_BOX_LENGTH && z_diff >= MIN_BOX_HEIGHT) {
            flag = true;
            temp_icpbcp_list.push_back(icp_bcp);
        }
    }
    icpbcp_list.clear();
    icpbcp_list = temp_icpbcp_list;
    open = flag;
}

int Bin::update_icpbcp_list(int icpbcp_idx, vector_3d dim) {
    try {
        vector_3d pos = icpbcp_list[icpbcp_idx].first;
        int x_o = pos.x;
        int y_o = pos.y;
        int z_o = pos.z;

        int w_o = dim.x;
        int l_o = dim.y;
        int h_o = dim.z;
        std::set<std::pair<vector_3d, vector_3d>> icpbcp_st;
        std::vector<std::pair<vector_3d, vector_3d>> temp_icpbcp_list;

        for (auto icpbcp : icpbcp_list) {
            int x_c1 = icpbcp.first.x;
            int y_c1 = icpbcp.first.y;
            int z_c1 = icpbcp.first.z;
            int x_c2 = icpbcp.second.x;
            int y_c2 = icpbcp.second.y;
            int z_c2 = icpbcp.second.z;
            if (is_overlapping(icpbcp, pos, dim)) {
                if (x_o > x_c1) {
                    icpbcp_st.insert({{x_c1, y_c1, z_c1}, {x_o, y_c2, z_c2}});
                }
                if (x_o + w_o < x_c2) {
                    icpbcp_st.insert({{x_o + w_o, y_c1, z_c1}, {x_c2, y_c2, z_c2}});
                }
                if (y_o > y_c1) {
                    icpbcp_st.insert({{x_c1, y_c1, z_c1}, {x_c2, y_o, z_c2}});
                }
                if (y_o + l_o < y_c2) {
                    icpbcp_st.insert({{x_c1, y_o + l_o, z_c1}, {x_c2, y_c2, z_c2}});
                }
                if (z_o > z_c1) {
                    icpbcp_st.insert({{x_c1, y_c1, z_c1}, {x_c2, y_c2, z_o}});
                }
                if (z_o + h_o < z_c2) {
                    icpbcp_st.insert({{x_c1, y_c1, z_o + h_o}, {x_c2, y_c2, z_c2}});
                }
            } else {
                temp_icpbcp_list.push_back(icpbcp);
            }
        }
        icpbcp_list.clear();
        icpbcp_list = temp_icpbcp_list;

        for (auto it : icpbcp_st) {
            icpbcp_list.push_back(it);
        }

        Bin::set_open();

    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 0;
    }

    return 1;
}
void Bin::print_state() {
    for (int i = 0; i < BIN_WIDTH; i++) {
        for (int j = 0; j < BIN_LENGTH; j++) {
            std::cout << bin_state[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
std::vector<std::vector<int>> Bin::get_state() {
    return bin_state;
}
std::vector<std::pair<vector_3d, vector_3d>> Bin::get_icbp_list() {
    return icpbcp_list;
}