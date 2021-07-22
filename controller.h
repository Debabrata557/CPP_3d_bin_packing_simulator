#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include "config.h"
#include "helper.h"
#ifndef controller
#define controller
class Controller {
   private:
        // int pre_computed_max[BIN_WIDTH][BIN_LENGTH][MAX_BOX_WIDTH][MAX_BOX_LENGTH];
        // int pre_computed_min[BIN_WIDTH][BIN_LENGTH][MAX_BOX_WIDTH][MAX_BOX_LENGTH];
        std::vector<std::vector<std::vector<std::vector<int>>>> pre_computed_max;
        std::vector<std::vector<std::vector<std::vector<int>>>> pre_computed_min;
        void precompute_max_min(const std::vector<std::vector<int>> &state);
        std::pair<int,int> compute_max_min(int x1, int x2, int y1, int y2);
    /* data */
   public:
    Controller(/* args */);
    ~Controller();
    bool check(const std::vector<std::vector<int>> &state, std::pair<int, int> pos, int lx, int ly, int lz);
    std::vector<int> first_fit(std::vector<std::vector<int>> &state, std::vector<int> &dim);
};
#endif
