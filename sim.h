#include <iostream>
#include <vector>

#include "config.h"

#ifndef sim
#define sim

// std::vector<std::vector<int>> bin_state(120, std::vector<int>(180, 0));

class Sim {
   private:
    std::vector<std::vector<int>> bin_state;

   public:
    Sim(/* args */);
    ~Sim();
    int update_state(std::pair<int, int> start_corner, int width, int length, int height);
    void print_state();
    std::vector<std::vector<int>> get_state();
};

#endif

// int main() {
//     Sim sim = Sim();
//     if (sim.update_state({0, 0}, 5, 5, 10)) {
//         sim.print_state();
//     }
//     return 0;
// }
