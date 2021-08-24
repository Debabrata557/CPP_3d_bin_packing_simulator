#include <iostream>
#include <vector>
#include <climits>
#include "config.h"
#include "bin.h"

#ifndef sim
#define sim

// std::vector<std::vector<int>> bin_state(120, std::vector<int>(180, 0));

class Sim
{
private:
    long long total_volume;
    int max_bin_limit;
    int max_open_limit;
    int cur_open_bins;

public:
    Sim();
    ~Sim();
    int total_number_of_boxes_placed;
    std::vector<Bin> bin_instances;
    int size_of_box_stream;
    int close_bin();
    void update_volume(vector_3d box);
    int open_new_bin();
    void set_limits(int bins_limit, int open_bins_limit);
    int step(int bin_id, int icp_bcp_list_id, vector_3d box, int orientation);
    performance_metric get_performance_metric();
};

#endif

// int main() {
//     Sim sim = Sim();
//     if (sim.update_state({0, 0}, 5, 5, 10)) {
//         sim.print_state();
//     }
//     return 0;
// }
