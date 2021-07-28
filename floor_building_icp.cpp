#include "base.h"

class Floor_Building_Icp : public Base
{
private:
    std::pair<int, int> get_action(std::vector<std::pair<vector_3d, vector_3d>> &icpbcp_list, std::vector<std::vector<int>> &state, vector_3d &dim)
    {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        std::sort(icpbcp_list.begin(), icpbcp_list.end());
        for (int i = 0; i < icpbcp_list.size(); i++)
        {
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim))
            {
                return {i, 0};
            }
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, {dim.y, dim.x, dim.z}))
            {
                return {i, 1};
            }
        }
        return {-1, 0};
    }

public:
    Floor_Building_Icp()
    {
    }
    Floor_Building_Icp(GenerateBox gb, Sim simulator) : Base(gb, simulator)
    {
    }
    double execute()
    {
        double total_volume = 0;
        int count = 0;
        for (auto box : boxes)
        {
            std::vector<std::vector<int>> cur_state = simulator.get_state();
            std::vector<std::pair<vector_3d, vector_3d>> icpbcp_list = simulator.get_icbp_list();
            //precompute_max_min(cur_state);
            auto idx_ori = get_action(icpbcp_list, cur_state, box);
            // std::cout << idx_ori.first << "\n";
            if (idx_ori.first >= 0)
            {
                if (idx_ori.second == 0)
                {
                    if (simulator.update_icpbcp_list(idx_ori.first, box) && simulator.update_state({icpbcp_list[idx_ori.first].first.x, icpbcp_list[idx_ori.first].first.y}, box))
                    {
                        total_volume += box.x * box.y * box.z;
                    }
                    else
                    {
                        std::cout << "exception occured" << std::endl;
                        return -1;
                    }
                }
                else
                {
                    if (simulator.update_icpbcp_list(idx_ori.first, {box.y, box.x, box.z}) && simulator.update_state({icpbcp_list[idx_ori.first].first.x, icpbcp_list[idx_ori.first].first.y}, {box.y, box.x, box.z}))
                    {
                        total_volume += box.x * box.y * box.z;
                    }
                    else
                    {
                        std::cout << "exception occured" << std::endl;
                        return -1;
                    }
                }
                //std::cout << "icpbcp_size " << icpbcp_list.size() << "\n";
            }
            else
            {
                //std::cout << "could not place the box" << std::endl;
            }

            // simulator.print_state();
            // std::cout << "box_passed"
            //           << " " << box[0] << " " << box[1] << " " << box[2] << " "
            //           << " "
            //           << "count: " << count++
            //           << "\n";
        }
        double efficiency = total_volume / (double)(BIN_WIDTH * BIN_HEIGHT * BIN_LENGTH);
        return efficiency;
    }
};