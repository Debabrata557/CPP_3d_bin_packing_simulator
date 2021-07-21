#include <iostream>
#include <vector>

#include "config.h"
#include "helper.h"
#ifndef controller
#define controller
class Controller {
   private:
    /* data */
   public:
    Controller(/* args */);
    ~Controller();
    bool check(const std::vector<std::vector<int>> &state, std::pair<int, int> pos, int lx, int ly, int lz);
    std::vector<int> first_fit(std::vector<std::vector<int>> &state, std::vector<int> &dim);
};
#endif
