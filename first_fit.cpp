#include "base.h"

class First_Fit : public Base {
   private:
    std::vector<int> get_action(std::vector<std::vector<int>> &state, vector_3d &dim) {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        // #print('checking new dim: ', lx, ',', ly, ',', lz)
        // #bin_packing_helper.printStates(state)
        //std::cout<<"precomputation starting"<<std::endl;
        //std::cout<<"precomputation done"<<std::endl;
        for (int i = 0; i < BIN_WIDTH; i++) {
            for (int j = 0; j < BIN_LENGTH; j++) {
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
    First_Fit() {
        pre_computed_max = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
        pre_computed_min = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
    }
    First_Fit(GenerateBox gb, std::vector<Bin> &bin_instances) : Base(gb, bin_instances) {
        pre_computed_max = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
        pre_computed_min = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
    }

    bool put_box(Bin &bin_instance, vector_3d box, double &total_volume) {
        std::vector<std::vector<int>> cur_state = bin_instance.get_state();
        precompute_max_min(cur_state);
        std::vector<int> pose = get_action(cur_state, box);
        if (pose[0] >= 0) {
            if (pose[2] == 0) {
                if (bin_instance.update_state({pose[0], pose[1]}, box)) {
                    total_volume += box.x * box.y * box.z;
                    return 1;
                } else {
                    std::cout << "exception occured" << std::endl;
                    return 0;
                }
            } else {
                if (bin_instance.update_state({pose[0], pose[1]}, {box.y, box.x, box.z})) {
                    total_volume += box.x * box.y * box.z;
                    return 1;
                } else {
                    std::cout << "exception occured" << std::endl;
                    return 0;
                }
            }
        } else {
            //a  std::cout << "could not place the box" << std::endl;
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