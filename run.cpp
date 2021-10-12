/*
./build/run test_dir/weights_1.txt test_dir/result_1.txt 0
*/

#include <fstream>
#include <future>
#include <thread>

#include "first_fit.cpp"
#include "first_fit_icp.cpp"
#include "floor_building.cpp"
#include "floor_building_icp.cpp"
#include "generate_box.h"
#include "random_algorithm.cpp"
#include "sim.h"
#include "smart_algo_withlookahead.cpp"
#include "smart_algorithm.cpp"
#include "smart_algorithm2.cpp"
#include "smart_algorithm_without_icp_bcp.cpp"

std::ifstream read_file;
std::ofstream write_file;

performance_metric worker(std::string algo_name, int seed, int episode, std::string read_file_name, std::string boxGeneration, int openBinCount)  //episode number is seed
{
    GenerateBox gb;
    if(boxGeneration=="cut1")
        gb = GenerateBox(seed, "cut1", 0);
    else
        gb = GenerateBox(seed, "random", 2000);
    
    read_file.open(read_file_name);
    std::vector<vector_3d> boxes = gb.get_stream_of_boxes();
    // std::cout << boxes.size() << "\n";
    int bin_limit = 1;
    int max_open_bin = openBinCount;
    int lookahead;
    Sim simulator = Sim();
    simulator.set_limits(bin_limit, max_open_bin);
    // int NPARAMS_X = (BIN_WIDTH+STRIDE-1)/STRIDE; //without symmetry
    // int NPARAMS_Y = (BIN_LENGTH+STRIDE-1)/STRIDE;
    // int NPARAMS_X = (EXTRACT_FEATURE_AREA+STRIDE-1)/STRIDE;
    // int NPARAMS_Y = (EXTRACT_FEATURE_AREA+STRIDE-1)/STRIDE; /// this should be integer.Discreteization factor must be a factor of BIN_WIDTH and BIN_LENGTH
    std::vector<double> params(TOTAL_PARAMS, 0);
    for (int i = 0; i < TOTAL_PARAMS; i++) {
        read_file >> params[i];
        // std::cout<<params[i]<<"\n";
    }
    Base* x;
    if (algo_name == "first_fit_icp") {
        std::cout << "running first_fit_icp.." << episode << " " << seed << "\n";
        x = new First_Fit_Icp(gb, simulator);
    }
    else if (algo_name == "smart_algo") {
        std::cout << "running smart algorithm.." << episode << " " << seed << "\n";
        x = new Smart_Algorithm(gb, simulator, params);
    }
    else if (algo_name == "smart_algo_with_lookahead") {
        std::cout << "running smart algorithm with lookahead.." << episode << " " << seed << "\n";
        x = new Smart_Algorithm_WithLookahead(gb, simulator, params);
        lookahead = 1;
    }
    // else if (algo_name == "floor_building") {
    //     std::cout << "running floor_building.." << episode << " " << seed << "\n";
    //     x = new Floor_Building(gb, simulator);
    // }
    // else  if (algo_name == "first_fit") {
    //     std::cout << "running first_fit.." << episode << " " << seed << "\n";
    //     x = new First_Fit(gb, simulator);
    // }
    // else if (algo_name == "floor_building_icp") {
    //     std::cout << "running floor_building_icp.." << episode << " " << seed << "\n";
    //     x = new Floor_Building_Icp(gb, simulator);
    // }
    // else if (algo_name == "smart_algo_without_icp_bcp") {
    //     std::cout << "running smart algorithm without icp bcp.." << episode << " " << seed << "\n";
    //     x = new Smart_Algorithm_WithoutICP_BCP(gb, simulator, params);
    //     lookahead = 0;
    // }
    //Base* x = new Smart_Algorithm_WithLookahead(gb, simulator, params);
    //Base* x = new Random_Algorithm(gb, simulator);
    //Base* x = new Smart_Algorithm2(gb, simulator, params);
    performance_metric pm = x->execute(simulator, lookahead);
    std::cout << pm.efficiency << std::endl;

    delete (x);
    read_file.close();
    return pm;
}

int main(int argc, char** argv) {
    std::string read_file_name = argv[1];

    std::string write_file_name = argv[2];
    std::string algo_name = argv[5];
    std::string boxGeneration = argv[6];
    int openBinCount = std::stoi(argv[7]);
    int debug = std::stoi(argv[8]);
    if (!debug)
        std::cout.setstate(std::ios_base::failbit);
    write_file.open(write_file_name);
    //std::thread threadHandles[episode];
    clock_t start_time = clock();
    int episode = std::stoi(argv[4]);
    ;
    int seed = std::stoi(argv[3]);
    //std::thread threadHandles[episode];
    double efficiency = 0;
    double no_of_bins = 0;
    double total_boxes = 0;
    double boxes_put = 0;
    int num_threads = 1;
    bool constant_seed = 0;
    double mineff = INT_MAX;
    int minseed = -1;
    for (int k = 0; k < episode / num_threads; k++) {
        std::vector<std::future<performance_metric>> threadHandles(num_threads);
        for (int i = 0; i < num_threads; i++) {
            threadHandles[i] = std::async(worker, algo_name, seed, k * num_threads + i, read_file_name, boxGeneration, openBinCount);
            if (!constant_seed) seed++;
            //threadHandles[i] = std::thread(worker, i);
        }

        for (long long i = 0; i < num_threads; i++) {
            performance_metric pm = threadHandles[i].get();
            if (pm.efficiency < mineff) {
                mineff = pm.efficiency;
                minseed = seed;
                //minep=k*num_threads+i;
            }
            efficiency += pm.efficiency;
            no_of_bins += pm.number_of_bins_used;
            total_boxes += pm.total_number_of_boxes;
            boxes_put += pm.number_of_boxes_successfully_put;
            //threadHandles[i].join();
        }
        // std::cout << k << "\n";
    }
    std::cout << "avg efficiency"
              << " " << efficiency / episode << " ";
    std::cout << "avg no of bins"
              << " " << no_of_bins / episode << " ";
    std::cout << "average total boxes"
              << " " << total_boxes / episode << " ";
    std::cout << "min efficiency"
              << " " << mineff << "\n";
    std::cout << "min seed"
              << " " << minseed-1 << "\n";

    clock_t end_time = clock();

    float time_passed = float(end_time - start_time) / (float)CLOCKS_PER_SEC;
    // std::cout << time_passed / num_threads << "\n";
    write_file << efficiency / episode << "\n";
    write_file << boxes_put / episode << "\n";
    // write_file << time_passed <<"\n";
    write_file.close();
    return 0;
    // Base* x = new First_Fit_Icp();
    // x->execute();
    // return 0;
}