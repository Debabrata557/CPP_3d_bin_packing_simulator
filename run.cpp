#include "first_fit.cpp"
#include "first_fit_icp.cpp"
#include "floor_building.cpp"
#include "floor_building_icp.cpp"
#include "generate_box.h"
#include "sim.h"
#include <thread>
#include <future>

double worker(int seed)
{
    GenerateBox gb = GenerateBox(seed, "cut1");
    std::vector<vector_3d> boxes = gb.get_stream_of_boxes();
    Sim simulator = Sim();
    // Base *x = new First_Fit(gb, simulator);
    Base *x = new First_Fit_Icp(gb, simulator);
    //Base *x = new Floor_building_Icp(gb, simulator);
    double efficiency = x->execute();
    // std::cout << "efficiency"
    //           << " " << efficiency << "\n";
    delete (x);
    return efficiency;
}
int main()
{
    int episode = 100;
    std::vector<std::future<double>> threadHandles(episode);
    //std::thread threadHandles[episode];
    clock_t start_time = clock();
    for (int k = 0; k < 1; k++)
    {
        for (int i = 0; i < episode; i++)
        {
            threadHandles[i] = std::async(worker, i);
            //threadHandles[i] = std::thread(worker, i);
        }
        double sum = 0;
        for (long long i = 0; i < episode; i++)
        {
            sum += threadHandles[i].get();
            //threadHandles[i].join();
        }
        std::cout << "avg efficiency"
                  << " " << sum / episode << "\n";
    }
    clock_t end_time = clock();
    float time_passed = float(end_time - start_time) / (float)CLOCKS_PER_SEC;
    std::cout << time_passed / 16.0 << "\n";
    return 0;
    // Base* x = new First_Fit_Icp();
    // x->execute();
    // return 0;
}