#include "base.h"

class Floor_Building_Icp : public Base {
   private:
    std::pair<int, int> get_action(std::vector<std::pair<vector_3d, vector_3d>> &icpbcp_list, std::vector<std::vector<int>> &state, vector_3d &dim) {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        // std::cout << "before sort" << icpbcp_list.size() << "\n";
        std::sort(icpbcp_list.begin(), icpbcp_list.end(), comp_floor_building);
        // std::cout << "after sort" << icpbcp_list.size() << "\n";
        for (int i = 0; i < icpbcp_list.size(); i++) {
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim)) {
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;

                if ((x_diff >= lx && y_diff >= ly && z_diff >= lz)) {
                    return {i, 0};
                }
            }
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, {dim.y, dim.x, dim.z})) {
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;

                if ((x_diff >= ly && y_diff >= lx && z_diff >= lz)) {
                    return {i, 1};
                }
            }
        }
        return {-1, 0};
    }

   public:
    Floor_Building_Icp() {
    }
    Floor_Building_Icp(GenerateBox gb, std::vector<Bin> &bin_instances) : Base(gb, bin_instances) {
    }

    bool put_box(Bin &bin_instance, vector_3d box, double &total_volume) {
        std::vector<std::vector<int>> cur_state = bin_instance.get_state();
        std::vector<std::pair<vector_3d, vector_3d>> icpbcp_list = bin_instance.get_icbp_list();
        //precompute_max_min(cur_state);
        auto idx_ori = get_action(icpbcp_list, cur_state, box);
        // std::cout << idx_ori.first << "\n";
        if (idx_ori.first >= 0) {
            if (idx_ori.second == 0) {
                if (bin_instance.update_icpbcp_list(idx_ori.first, box) && bin_instance.update_state({icpbcp_list[idx_ori.first].first.x, icpbcp_list[idx_ori.first].first.y}, box)) {
                    total_volume += box.x * box.y * box.z;
                    bin_instance.volume += box.x * box.y * box.z;
                    bin_instance.no_of_boxes_placed++;
                    return 1;
                } else {
                    std::cout << "exception occured" << std::endl;
                    return 0;
                }
            } else {
                if (bin_instance.update_icpbcp_list(idx_ori.first, {box.y, box.x, box.z}) && bin_instance.update_state({icpbcp_list[idx_ori.first].first.x, icpbcp_list[idx_ori.first].first.y}, {box.y, box.x, box.z})) {
                    total_volume += box.x * box.y * box.z;
                    bin_instance.volume += box.x * box.y * box.z;
                    bin_instance.no_of_boxes_placed++;
                    return 1;
                } else {
                    std::cout << "exception occured" << std::endl;
                    return 0;
                }
            }
            //std::cout << "icpbcp_size " << icpbcp_list.size() << "\n";
        } else {
            //std::cout << "could not place the box" << std::endl;
        }
        return 0;
    }
    performance_metric execute(int max_bin_limit, int max_open_bins) {
        double total_volume = 0;
        double no_of_bins_used = 0;
        double no_of_boxes_put = 0;
        performance_metric pm;
        int to_be_closed = 0;
        int cur_open = 0;
        for (auto box : boxes) {
            // std::cout << box.x << " " << box.y << " " << box.z << "\n";
            int flag = 0;
            for (int i = 0; i < bin_instances.size(); i++) {
                if (bin_instances[i].is_open()) {
                    if (put_box(bin_instances[i], box, total_volume)) {
                        flag = 1;
                        no_of_boxes_put++;
                        break;
                    }
                }
            }
            if (!flag && no_of_bins_used < max_bin_limit) {
                Bin new_bin = Bin();
                if (put_box(new_bin, box, total_volume)) {
                    no_of_boxes_put++;
                }
                bin_instances.push_back(new_bin);
                no_of_bins_used++;
                cur_open++;
                if (cur_open > max_open_bins) {
                    bin_instances[to_be_closed].open = false;
                    to_be_closed++;
                    cur_open--;
                }
            }
        }

        // for (int i = 1; i <= bin_instances.size(); i++) {
        //     std::cout << i << " " << bin_instances[i - 1].no_of_boxes_placed << " " << bin_instances[i - 1].volume / (BIN_WIDTH * BIN_HEIGHT * BIN_LENGTH) << "\n";
        // }

        double efficiency = total_volume / (double)(no_of_bins_used * BIN_WIDTH * BIN_HEIGHT * BIN_LENGTH);
        pm.efficiency = efficiency;
        pm.number_of_bins_used = no_of_bins_used;
        pm.number_of_boxes_successfully_put = no_of_boxes_put;
        pm.total_number_of_boxes = boxes.size();
        return pm;
    }
};