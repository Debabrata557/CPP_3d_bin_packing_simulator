#include <future>
#include <thread>

#include "bin.h"
#include "first_fit.cpp"
#include "first_fit_icp.cpp"
#include "floor_building.cpp"
#include "floor_building_icp.cpp"
#include "generate_box.h"

performance_metric worker(int seed) {
    GenerateBox gb = GenerateBox(seed, "random", 200);
    std::vector<vector_3d> boxes = gb.get_stream_of_boxes();
    std::vector<Bin> bin_instances;
    int bin_limit = 1;
    // Base *x = new First_Fit(gb, bin_instances);
    // Base *x = new First_Fit_Icp(gb, bin_instances);
    // Base *x = new Floor_Building_Icp(gb, bin_instances);
    Base *x = new Floor_Building(gb, bin_instances);
    performance_metric pm = x->execute(bin_limit);
    // std::cout << "efficiency"
    //           << " " << efficiency << "\n";
    delete (x);
    return pm;
}
int main() {
    int episode = 1;
    std::vector<std::future<performance_metric>> threadHandles(episode);
    //std::thread threadHandles[episode];
    clock_t start_time = clock();
    for (int k = 0; k < 1; k++) {
        for (int i = 0; i < episode; i++) {
            threadHandles[i] = std::async(worker, i);
            //threadHandles[i] = std::thread(worker, i);
        }
        double efficiency = 0;
        double no_of_bins = 0;
        double total_boxes = 0;
        double boxes_put = 0;
        for (long long i = 0; i < episode; i++) {
            performance_metric pm = threadHandles[i].get();
            efficiency += pm.efficiency;
            no_of_bins += pm.number_of_bins_used;
            total_boxes += pm.total_number_of_boxes;
            boxes_put += pm.number_of_boxes_successfully_put;
            //threadHandles[i].join();
        }
        std::cout << "avg efficiency"
                  << " " << efficiency / episode << " ";
        std::cout << "avg no of bins"
                  << " " << no_of_bins / episode << " ";
        std::cout << "average total boxes"
                  << " " << total_boxes / episode << " ";
        std::cout << "avg no of boxes put"
                  << " " << boxes_put / episode << "\n";
    }
    clock_t end_time = clock();
    float time_passed = float(end_time - start_time) / (float)CLOCKS_PER_SEC;
    std::cout << time_passed / 16.0 << "\n";
    return 0;
    // Base* x = new First_Fit_Icp();
    // x->execute();
    // return 0;
}