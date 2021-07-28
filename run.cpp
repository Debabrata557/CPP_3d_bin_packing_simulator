//#include "first_fit.cpp"
//#include "first_fit_icp.cpp"
#include "floor_building.cpp"

void *worker()
{
    //Base *x = new First_Fit();
    //Base *x = new First_Fit_Icp();
    Base *x = new Floor_Building();
    x->execute();
    delete (x);
    return 0;
}
int main()
{
    int episode = 100;
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
    std::cout << time_passed << "\n";
    return 0;
    // Base* x = new First_Fit_Icp();
    // x->execute();
    // return 0;
}