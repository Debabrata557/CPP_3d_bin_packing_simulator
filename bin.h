#include <iostream>
#include <vector>

#include "config.h"

#ifndef bin
#define bin

// std::vector<std::vector<int>> bin_state(120, std::vector<int>(180, 0));

class Bin
{
private:
    std::vector<std::vector<int>> bin_state;
    std::vector<std::pair<vector_3d, vector_3d>> icpbcp_list;
    bool is_overlapping(std::pair<vector_3d, vector_3d> icpbcp, vector_3d size, vector_3d pos);
    void set_open();

public:
    Bin(/* args */);
    ~Bin();
    bool open;
    bool recentlyChanged;
    int update_state(std::pair<int, int> start_corner, vector_3d dim);
    int update_icpbcp_list(int icpbcp_idx, vector_3d dim);
    void print_state();
    bool is_open();
    double volume;
    double no_of_boxes_placed;
    std::vector<std::vector<int>> get_state();
    std::vector<std::pair<vector_3d, vector_3d>> get_icbp_list();
};

#endif

// int main() {
//     Sim sim = Sim();
//     if (sim.update_state({0, 0}, 5, 5, 10)) {
//         sim.print_state();
//     }
//     return 0;
// }
