#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iostream>
#include <thread>
#include <tuple>
#include <vector>
#include <float.h>
#include "bin.h"
#include "config.h"
#include "generate_box.h"
#include "helper.h"
#include "sim.h"

#ifndef base
#define base

// std::vector<std::vector<int>> bin_state(120, std::vector<int>(180, 0));

class Base
{
protected:
    GenerateBox gb;
    //std::vector<Bin> bin_instances;
    //Sim simulator;
    std::vector<vector_3d> boxes;
    std::vector<std::vector<std::vector<std::vector<int>>>> pre_computed_max;
    std::vector<std::vector<std::vector<std::vector<int>>>> pre_computed_min;
    std::pair<int, int> compute_max_min(int x1, int x2, int y1, int y2);
    void precompute_max_min(const std::vector<std::vector<int>> &state);

public:
    Base(/* args */);
    Base(GenerateBox gb, Sim &simulator);
    ~Base();
    virtual performance_metric execute(Sim &simulator, int lookahead) = 0;
    bool check_with_precomputation(const std::vector<std::vector<int>> &state, std::pair<int, int> pos, vector_3d dim);
    bool check_without_precomputation(const std::vector<std::vector<int>> &state, std::pair<int, int> pos, vector_3d dim);
};

#endif

// int main() {
//     Sim sim = Sim();
//     if (sim.update_state({0, 0}, 5, 5, 10)) {
//         sim.print_state();
//     }
//     return 0;
// }
