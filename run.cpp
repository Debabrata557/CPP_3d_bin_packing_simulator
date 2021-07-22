#include <ctime>

#include "controller.h"
#include "generate_box.h"
#include "sim.h"
#include <unistd.h>
#include <thread>

void* worker(){
    GenerateBox gb = GenerateBox();
    Sim simulator = Sim();
    Controller agent = Controller();
    std::vector<std::vector<int>> boxes;
    if (gb.generate_cut1()) {
        boxes = gb.get_stream_of_boxes();
    }
    double total_volume = 0;
    int count = 0;
    for (auto box : boxes) {
        std::vector<std::vector<int>> cur_state = simulator.get_state();
       
        std::vector<int> pose = agent.first_fit(cur_state, box);
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
            // std::cout << "could not place the box" << std::endl;
        }

        // simulator.print_state();
        // std::cout << "box_passed"
        //           << " " << box[0] << " " << box[1] << " " << box[2] << " "
        //           << " "
        //           << "count: " << count++
        //           << "\n";
    }
    double efficiency = total_volume / (double)(BIN_WIDTH * BIN_HEIGHT * BIN_LENGTH);
    std::cout << "efficiency"
              << " " << efficiency << "\n";
}
int main() {
    int episode=15;
    std::thread threadHandles[episode];
    clock_t start_time = clock();
    for(int i=0;i<episode;i++){
        threadHandles[i]=std::thread(worker);
    }
    for(long long i=0; i < episode; i++){
		threadHandles[i].join();
	}
    clock_t end_time = clock();
    float time_passed = float(end_time - start_time) / (float)CLOCKS_PER_SEC;
    std::cout << time_passed << "\n";
    return 0;
}