#include "base.h"
#include <fstream>
class First_Fit : public Base
{
private:
    std::ofstream write_file;
    std::string write_file_name = "details.txt";
    std::vector<int> get_action(std::vector<std::vector<int>> &state, vector_3d &dim)
    {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        // #print('checking new dim: ', lx, ',', ly, ',', lz)
        // #bin_packing_helper.printStates(state)
        //std::cout<<"precomputation starting"<<std::endl;
        //std::cout<<"precomputation done"<<std::endl;
        for (int i = 0; i < BIN_WIDTH; i++)
        {
            for (int j = 0; j < BIN_LENGTH; j++)
            {
                int flag = check_with_precomputation(state, {i, j}, {lx, ly, lz});
                if (flag)
                    return {i, j, 0};
                flag = check_with_precomputation(state, {i, j}, {ly, lx, lz});
                if (flag)
                    return {i, j, 1};
            }
        }
        return {-40, -40, 0};
    }

public:
    First_Fit()
    {
        pre_computed_max = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
        pre_computed_min = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
    }
    First_Fit(GenerateBox gb, Sim &simulator) : Base(gb, simulator)
    {
        pre_computed_max = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
        pre_computed_min = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
    }

    bool put_box(Sim &simulator, int bin_id, vector_3d box)
    {
        std::vector<std::vector<int>> cur_state = simulator.bin_instances[bin_id].get_state();
        precompute_max_min(cur_state);
        std::vector<int> pose = get_action(cur_state, box);
        if (pose[0] >= 0)
        {
            int height=simulator.step(bin_id, {pose[0], pose[1]}, box, pose[2]);
            write_file<<bin_id<<" "<<box.x<<" "<<box.y<<" "<<box.z<<" "<<pose[0]<<" "<<pose[1]<<" "<<height<<" "<<pose[2]<<"\n";
            return height!=-1;
        }
        else
        {
            write_file<<bin_id<<" "<<box.x<<" "<<box.y<<" "<<box.z<<" "<<pose[0]<<" "<<pose[1]<<" "<<-1<<" "<<-1<<"\n";
            //a  std::cout << "could not place the box" << std::endl;
        }
        return 0;
    }
    performance_metric execute(Sim &simulator, int lookahead)
    {
        write_file.open(write_file_name);
        simulator.size_of_box_stream = boxes.size();
        for (auto box : boxes)
        {
            // std::cout << box.x << " " << box.y << " " << box.z << "\n";
            int flag = 0;
            for (int i = 0; i < simulator.bin_instances.size(); i++)
            {
                if (simulator.bin_instances[i].is_open())
                {
                    if (put_box(simulator, i, box))
                    {
                        flag = 1;
                        break;
                    }
                }
            }
            if (!flag && simulator.open_new_bin())
            {
                put_box(simulator, simulator.bin_instances.size() - 1, box);
            }
        }
        write_file.close();
        return simulator.get_performance_metric(1);
    }
};