#include "base.h"

class Floor_Building_Icp : public Base {
   private:
    std::pair<int, int> get_action(std::vector<std::pair<vector_3d, vector_3d>> &icpbcp_list, std::vector<std::vector<int>> &state, vector_3d &dim) {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        std::sort(icpbcp_list.begin(), icpbcp_list.end(), comp_floor_building);
        for (int i = 0; i < icpbcp_list.size(); i++) {
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim)) {
                return {i, 0};
            }
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, {dim.y, dim.x, dim.z})) {
                return {i, 1};
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
                    return 1;
                } else {
                    std::cout << "exception occured" << std::endl;
                    return 0;
                }
            } else {
                if (bin_instance.update_icpbcp_list(idx_ori.first, {box.y, box.x, box.z}) && bin_instance.update_state({icpbcp_list[idx_ori.first].first.x, icpbcp_list[idx_ori.first].first.y}, {box.y, box.x, box.z})) {
                    total_volume += box.x * box.y * box.z;
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
    performance_metric execute(int max_bin_limit) {
        double total_volume = 0;
        double no_of_bins_used = 0;
        double no_of_boxes_put = 0;
        performance_metric pm;
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
            }
        }

        double efficiency = total_volume / (double)(no_of_bins_used * BIN_WIDTH * BIN_HEIGHT * BIN_LENGTH);
        pm.efficiency = efficiency;
        pm.number_of_bins_used = no_of_bins_used;
        pm.number_of_boxes_successfully_put = no_of_boxes_put;
        pm.total_number_of_boxes = boxes.size();
        return pm;
    }
};