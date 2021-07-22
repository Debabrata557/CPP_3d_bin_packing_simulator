#include <ctime>

#include "controller.h"
#include "generate_box.h"
#include "sim.h"
#include <unistd.h>

int main() {
    GenerateBox gb = GenerateBox();
    Sim simulator = Sim();
    Controller agent = Controller();
    std::vector<std::vector<int>> boxes;
    if (gb.generate_cut1()) {
        boxes = gb.get_stream_of_boxes();
    }
    double total_volume = 0;
    float total_time = 0;
    int count = 0;
    for (auto box : boxes) {
        std::vector<std::vector<int>> cur_state = simulator.get_state();
        clock_t start_time = clock();
        std::vector<int> pose = agent.first_fit(cur_state, box);
        clock_t end_time = clock();
        float time_passed = float(end_time - start_time) / (float)CLOCKS_PER_SEC;
        std::cout << time_passed << "\n";
        total_time += time_passed;
        if (pose[0] >= 0) {
            if (pose[2] == 0) {
                if (simulator.update_state({pose[0], pose[1]}, box[0], box[1], box[2])) {
                    total_volume += box[0] * box[1] * box[2];
                }
            } else {
                if (simulator.update_state({pose[0], pose[1]}, box[1], box[0], box[2])) {
                    total_volume += box[0] * box[1] * box[2];
                }
            }
        } else {
            std::cout << "could not place the box" << std::endl;
        }

        // simulator.print_state();
        std::cout << "box_passed"
                  << " " << box[0] << " " << box[1] << " " << box[2] << " "
                  << " "
                  << "count: " << count++
                  << "\n";
    }
    double efficiency = total_volume / (double)(BIN_WIDTH * BIN_HEIGHT * BIN_LENGTH);
    std::cout << "efficiency"
              << " " << efficiency << "\n";
    std::cout << "total_time"
              << " " << total_time << std::endl;
    return 0;
}