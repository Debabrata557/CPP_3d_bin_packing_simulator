#include "first_fit.cpp"
#include "first_fit_icp.cpp"
#include "floor_building.cpp"
#include "floor_building_icp.cpp"
#include "generate_box.h"
#include "sim.h"

void *worker()
{
    int seed = 1;
    GenerateBox gb = GenerateBox(seed, "cut1");
    std::vector<vector_3d> boxes = gb.get_stream_of_boxes();
    std::cout << "box stream size: " << boxes.size() << std::endl;

    std::cout << "box stream size: " << boxes[0].x << " " << boxes[0].y << " " << boxes[0].z << std::endl;
    Sim simulator = Sim();
    // Base *x = new First_Fit(gb, simulator);
    Base *x = new First_Fit_Icp(gb, simulator);
    //Base *x = new Floor_building_Icp(gb, simulator);
    x->execute();
    delete (x);
    return 0;
}
int main()
{
    int episode = 1;
    std::thread threadHandles[episode];
    clock_t start_time = clock();
    for (int k = 0; k < 1; k++)
    {
        for (int i = 0; i < episode; i++)
        {
            threadHandles[i] = std::thread(worker);
        }
        for (long long i = 0; i < episode; i++)
        {
            threadHandles[i].join();
        }
    }
    clock_t end_time = clock();
    float time_passed = float(end_time - start_time) / (float)CLOCKS_PER_SEC;
    std::cout << time_passed / 16.0 << "\n";
    return 0;
    // Base* x = new First_Fit_Icp();
    // x->execute();
    // return 0;
}