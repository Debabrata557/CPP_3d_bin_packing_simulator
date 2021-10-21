#include <algorithm>
#include <fstream>
#include "base.h"

class Floor_Building_Icp : public Base
{
private:
    std::ofstream write_file;
    std::string write_file_name = "details.txt";
    std::pair<int, int> get_action(std::vector<vector_3d> &icpbcp_list, std::vector<std::vector<int>> &state, vector_3d &dim)
    {
        int min_icp_bcp=-1, min_height=INT_MAX, orientation=-1;
        // std::sort(icpbcp_list.begin(), icpbcp_list.end());
        for (int i = 0; i < icpbcp_list.size(); i++)
        {
            int x=icpbcp_list[i].x, y=icpbcp_list[i].y;
            if (check_without_precomputation(state, icpbcp_list[i], dim))
            {
                if(state[x][y]<min_height){
                    min_icp_bcp=i;
                    min_height=state[x][y];
                    orientation=0;
                }
            }
            if (check_without_precomputation(state, icpbcp_list[i], {dim.y, dim.x, dim.z}))
            {
                if(state[x][y]<min_height){
                    min_icp_bcp=i;
                    min_height=state[x][y];
                    orientation=1;
                }
            }
        }
        return {min_icp_bcp, orientation};
    }

public:
    Floor_Building_Icp()
    {
    }
    Floor_Building_Icp(GenerateBox gb, Sim &simulator) : Base(gb, simulator)
    {
    }
    bool put_box(Sim &simulator, int bin_id, vector_3d box, int real)
    {
        std::vector<std::vector<int>> cur_state = simulator.bin_instances[bin_id].get_state();
        // bin_instance.print_state();
        std::vector<vector_3d> &icpbcp_list = simulator.bin_instances[bin_id].get_icbp_list();
        //precompute_max_min(cur_state);
        auto idx_ori = get_action(icpbcp_list, cur_state, box);
        auto icp_bcp=simulator.bin_instances[bin_id].get_icbp_list()[idx_ori.first];
        // std::cout << idx_ori.first << "\n";
        if (idx_ori.first >= 0)
        {   
            int height=simulator.step(bin_id, idx_ori.first, box, idx_ori.second);
            if(real){
                auto start_point=get_start_point(icp_bcp, idx_ori.second, box);
                write_file << bin_id << " " << box.x << " " << box.y << " " << box.z << " " << start_point.first << " " << start_point.second << " " << height << " " << idx_ori.second << "\n";   
            }
            return height!=-1;
        }
        else
        {
            if(real)
                write_file<<bin_id<<" "<<box.x<<" "<<box.y<<" "<<box.z<<" "<<icp_bcp.x<<" "<<icp_bcp.y<<" "<<-1<<" "<<-1<<"\n"; 
            //std::cout << "could not place the box" << std::endl;
        }
        return 0;
    }

    performance_metric execute(Sim &simulator, int lookahead) {
        simulator.size_of_box_stream=boxes.size();
        write_file.open(write_file_name);
        for (auto box : boxes) {
            // std::cout << box.x << " " << box.y << " " << box.z << "\n";
            int flag = 0;
            for (int i = 0; i < simulator.bin_instances.size(); i++) {
                if (simulator.bin_instances[i].is_open()) {
                    if (put_box(simulator, i, box,1)) {
                        // std::cout<<"put"<<"\n";
                        flag = 1;
                        break;
                    }
                }
            }
            if (!flag && simulator.open_new_bin()) {
                put_box(simulator, simulator.bin_instances.size() - 1, box,1);
            }
        }
        write_file.close();

        return simulator.get_performance_metric(1);
    }
};