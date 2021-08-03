#include "base.h"

class Floor_Building : public Base {
   private:
    int score(const std::vector<std::vector<int>> &state, const std::pair<int, int> &pos, const vector_3d &dim) {
        int lx = dim.x;
        int ly = dim.y;

        int minx = pos.first;
        int maxx = pos.first + lx - 1;
        int miny = pos.second;
        int maxy = pos.second + ly - 1;
        int max_height = compute_max_min(minx, maxx, miny, maxy).first;
        return max_height;
    }
    std::vector<int> get_action(std::vector<std::vector<int>> &state, vector_3d &dim) {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        vector_3d pos_orientation = {-40, -40, 0};
        int min_height = 200;
        // #print('checking new dim: ', lx, ',', ly, ',', lz)
        // #bin_packing_helper.printStates(state)
        //std::cout<<"precomputation starting"<<std::endl;
        //std::cout<<"precomputation done"<<std::endl;
        for (int i = 0; i < BIN_WIDTH; i++) {
            for (int j = 0; j < BIN_LENGTH; j++) {
                int flag = check_with_precomputation(state, {i, j}, {lx, ly, lz});
                if (flag && min_height > score(state, {i, j}, {lx, ly, lz})) {
                    pos_orientation = {i, j, 0};
                }
                flag = check_with_precomputation(state, {i, j}, {ly, lx, lz});
                if (flag && min_height > score(state, {i, j}, {ly, lx, lz})) {
                    pos_orientation = {i, j, 1};
                }
            }
        }
        return {pos_orientation.x, pos_orientation.y, pos_orientation.z};
    }

   public:
    Floor_Building() {
        pre_computed_max = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
        pre_computed_min = std::vector<std::vector<std::vector<std::vector<int>>>>(BIN_WIDTH + 1, std::vector<std::vector<std::vector<int>>>(BIN_LENGTH + 1, std::vector<std::vector<int>>(10, std::vector<int>(10))));
    }
    Floor_Building(GenerateBox gb, Bin bin_instance) : Base(gb, bin_instance) {
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