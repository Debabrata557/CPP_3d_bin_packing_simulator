#include <fstream>
#include <future>
#include <thread>

#include "first_fit.cpp"
#include "first_fit_icp.cpp"
#include "floor_building.cpp"
#include "floor_building_icp.cpp"
#include "generate_box.h"
#include "sim.h"
#include "smart_algorithm.cpp"

std::ifstream read_file;
std::ofstream write_file;

performance_metric worker(int seed)//episode number is seed
{
    // GenerateBox gb = GenerateBox(seed, "random", 2000);
    GenerateBox gb = GenerateBox(seed, "cut1", 0);
    std::vector<vector_3d> boxes = gb.get_stream_of_boxes();
    // std::cout << boxes.size() << "\n";
    int bin_limit = 1;
    int max_open_bin = 1;
    int lookahead = 1;
    Sim simulator = Sim();
    //simulator.set_limits(bin_limit, max_open_bin);
    // Base *x = new First_Fit(gb, bin_instances);
    // Base *x = new First_Fit_Icp(gb, simulator);
    // Base *x = new Floor_Building_Icp(gb, bin_instances);
    // Base *x = new Floor_Building(gb, bin_instances);
    int params_size = 3*((BIN_LENGTH*BIN_WIDTH)/(FILTER_SIZE*FILTER_SIZE))+1; /// this should be integer.Discreteization factor must be a factor of BIN_WIDTH and BIN_LENGTH
    std::vector<double>params(params_size,0);
    for(int i=0;i<params_size;i++){
        read_file>>params[i];
        // std::cout<<params[i]<<"\n";
    }
    Base *x = new Smart_Algorithm(gb, simulator,params);
    performance_metric pm = x->execute(simulator, lookahead);
    // std::cout << bin_instances.size() << "\n";
    // for (int i = 1; i <= bin_instances.size(); i++) {
    //     std::cout << i << " " << bin_instances[i - 1].no_of_boxes_placed << " " << bin_instances[i - 1].volume << "\n";
    // }
    // std::cout << "efficiency"
    //           << " " << efficiency << "\n";
    delete (x);
    return pm;
}


int main(int argc,char** argv)
{
    std::string read_file_name = argv[1];

    std::string write_file_name = argv[2];


    
    read_file.open(read_file_name);
    write_file.open(write_file_name);
    //std::thread threadHandles[episode];
    clock_t start_time = clock();
    int episode = 20;
    int seed = std::stoi(argv[3]);
    //std::thread threadHandles[episode];
    double efficiency = 0;
    double no_of_bins = 0;
    double total_boxes = 0;
    double boxes_put = 0;
    int num_threads = 4;
    for (int k = 0; k < episode / num_threads; k++)
    {
        std::vector<std::future<performance_metric>> threadHandles(num_threads);
        for (int i = 0; i < num_threads; i++)
        {
            threadHandles[i] = std::async(worker, seed++);
            //threadHandles[i] = std::thread(worker, i);
        }

        for (long long i = 0; i < num_threads; i++)
        {
            performance_metric pm = threadHandles[i].get();
            efficiency += pm.efficiency;
            no_of_bins += pm.number_of_bins_used;
            total_boxes += pm.total_number_of_boxes;
            boxes_put += pm.number_of_boxes_successfully_put;
            //threadHandles[i].join();
        }
        // std::cout << k << "\n";
    }
    // std::cout << "avg efficiency"
    //           << " " << efficiency / episode << " ";
    // std::cout << "avg no of bins"
    //           << " " << no_of_bins / episode << " ";
    // std::cout << "average total boxes"
    //           << " " << total_boxes / episode << " ";
    // std::cout << "avg no of boxes put"
    //           << " " << boxes_put / episode << "\n";

    
    clock_t end_time = clock();
    
    float time_passed = float(end_time - start_time) / (float)CLOCKS_PER_SEC;
    // std::cout << time_passed / num_threads << "\n";
    write_file << efficiency / episode << "\n";
    write_file << boxes_put / episode << "\n";
    // write_file << time_passed <<"\n";
    read_file.close();
    write_file.close();
    return 0;
    // Base* x = new First_Fit_Icp();
    // x->execute();
    // return 0;
}