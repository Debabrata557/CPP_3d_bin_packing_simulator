#include "bin.h"

#include <set>

Bin::Bin(/* args */) {
    bin_state = std::vector<std::vector<int>>(BIN_WIDTH, std::vector<int>(BIN_LENGTH, 0));
    // icpbcp_list = std::vector<vector_3d>({{0, 0, 0},{0, BIN_LENGTH-1,1}, {BIN_WIDTH-1, 0, 2}, {BIN_WIDTH-1, BIN_LENGTH-1, 3}});
    // icpbcp_list = std::vector<vector_3d>({{0, 0, 0}});
    icpbcp_list=std::vector<vector_3d>(BIN_WIDTH*BIN_LENGTH);
    for(int i=0;i<BIN_WIDTH;i++){
        for(int j=0;j<BIN_LENGTH;j++){
            icpbcp_list[i*BIN_LENGTH+j]={i,j,0};
        }
    }
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
        return -1;
    }

    return cur_max_height - height;
}

int Bin::update_icpbcp_list(vector_3d pos, vector_3d dim) {
   /* try {
        // vector_3d pos = icpbcp_list[icpbcp_idx].first;
        int x_o = pos.x;
        int y_o = pos.y;
        int corner = pos.z;

        int w_o = dim.x;
        int l_o = dim.y;
        int h_o = dim.z;
        if (corner == 0) {
            // if (x_o + w_o < BIN_WIDTH && bin_state[x_o + w_o][y_o] < BIN_HEIGHT && bin_state[x_o + w_o][y_o] != bin_state[x_o][y_o]) icpbcp_list.push_back({x_o + w_o, y_o, 0});
            // if (y_o + l_o < BIN_LENGTH && bin_state[x_o][y_o + l_o] < BIN_HEIGHT && bin_state[x_o][y_o + l_o] != bin_state[x_o][y_o]) icpbcp_list.push_back({x_o, y_o + l_o, 0});
            if (x_o + w_o < BIN_WIDTH) icpbcp_list.push_back({x_o + w_o, y_o, 0});
            if (y_o + l_o < BIN_LENGTH) icpbcp_list.push_back({x_o, y_o + l_o, 0});
        } else if (corner == 1) {
            if (x_o + w_o < BIN_WIDTH && bin_state[x_o + w_o][y_o] < BIN_HEIGHT && bin_state[x_o + w_o][y_o] != bin_state[x_o][y_o]) icpbcp_list.push_back({x_o + w_o, y_o, 1});
            if (y_o - l_o >= 0 && bin_state[x_o][y_o - l_o] < BIN_HEIGHT && bin_state[x_o][y_o - l_o] != bin_state[x_o][y_o]) icpbcp_list.push_back({x_o, y_o - l_o, 1});
        } else if (corner == 2) {
            if (x_o - w_o >= 0 && bin_state[x_o - w_o][y_o] < BIN_HEIGHT && bin_state[x_o - w_o][y_o] != bin_state[x_o][y_o]) icpbcp_list.push_back({x_o - w_o, y_o, 2});
            if (y_o + l_o < BIN_LENGTH && bin_state[x_o][y_o + l_o] < BIN_HEIGHT && bin_state[x_o][y_o + l_o] != bin_state[x_o][y_o]) icpbcp_list.push_back({x_o, y_o + l_o, 2});

        } else {
            if (x_o - w_o >= 0 && bin_state[x_o - w_o][y_o] < BIN_HEIGHT && bin_state[x_o - w_o][y_o] != bin_state[x_o][y_o]) icpbcp_list.push_back({x_o - w_o, y_o, 3});
            if (y_o - l_o >= 0 && bin_state[x_o][y_o - l_o] < BIN_HEIGHT && bin_state[x_o][y_o - l_o] != bin_state[x_o][y_o]) icpbcp_list.push_back({x_o, y_o - l_o, 3});
        }
        // std::cout<<icpbcp_list.size()<<"\n";

    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
*/
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
std::vector<vector_3d> &Bin::get_icbp_list() {
    return icpbcp_list;
}