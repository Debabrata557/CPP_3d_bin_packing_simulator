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
    First_Fit(GenerateBox gb, Bin bin_instance) : Base(gb, bin_instance) {
        pre_computed_max = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
        pre_computed_min = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
    }
    double execute() {
        double total_volume = 0;
        int count = 0;
        for (auto box : boxes) {
            std::vector<std::vector<int>> cur_state = bin_instance.get_state();
            precompute_max_min(cur_state);
            std::vector<int> pose = get_action(cur_state, box);
            if (pose[0] >= 0) {
                if (pose[2] == 0) {
                    if (bin_instance.update_state({pose[0], pose[1]}, box)) {
                        total_volume += box.x * box.y * box.z;
                    } else {
                        std::cout << "exception occured" << std::endl;
                        return -1;
                    }
                } else {
                    if (bin_instance.update_state({pose[0], pose[1]}, {box.y, box.x, box.z})) {
                        total_volume += box.x * box.y * box.z;
                    } else {
                        std::cout << "exception occured" << std::endl;
                        return -1;
                    }
                }
            } else {
                //a  std::cout << "could not place the box" << std::endl;
            }

            // bin.print_state();
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