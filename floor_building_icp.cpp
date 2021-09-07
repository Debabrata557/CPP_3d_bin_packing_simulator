#include "base.h"

class Floor_Building_Icp : public Base
{
private:
    std::pair<int, int> get_action(std::vector<std::pair<vector_3d, vector_3d>> &icpbcp_list, std::vector<std::vector<int>> &state, vector_3d &dim)
    {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        // std::cout << "before sort" << icpbcp_list.size() << "\n";
        //std::sort(icpbcp_list.begin(), icpbcp_list.end(), comp_floor_building);
        // std::cout << "after sort" << icpbcp_list.size() << "\n";
        for (int i = 0; i < icpbcp_list.size(); i++)
        {
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim))
            {
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;

                if ((x_diff >= lx && y_diff >= ly && z_diff >= lz))
                {
                    return {i, 0};
                }
            }
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, {dim.y, dim.x, dim.z}))
            {
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;

                if ((x_diff >= ly && y_diff >= lx && z_diff >= lz))
                {
                    return {i, 1};
                }
            }
        }
        return {-1, 0};
    }

public:
    Floor_Building_Icp()
    {
    }
    Floor_Building_Icp(GenerateBox gb, Sim &simulator) : Base(gb, simulator)
    {
    }
    bool put_box(Sim &simulator, int bin_id, vector_3d box)
    {
        std::vector<std::vector<int>> cur_state = simulator.bin_instances[bin_id].get_state();
        // bin_instance.print_state();
        std::vector<std::pair<vector_3d, vector_3d>> icpbcp_list = simulator.bin_instances[bin_id].get_icbp_list();
        //precompute_max_min(cur_state);
        auto idx_ori = get_action(icpbcp_list, cur_state, box);

        // std::cout << idx_ori.first << "\n";
        if (idx_ori.first >= 0)
        {
            return simulator.step(bin_id, idx_ori.first, box, idx_ori.second);
        }
        else
        {
            //std::cout << "could not place the box" << std::endl;
        }
        return 0;
    }
    performance_metric execute(Sim &simulator, int lookahead)
    {
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

        return simulator.get_performance_metric(1);
    }
};