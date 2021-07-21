#include <iostream>
#include <vector>

#include "config.h"

// std::vector<std::vector<int>> bin_state(120, std::vector<int>(180, 0));

class Sim {
   private:
    int x, y;
    std::vector<std::vector<int>> bin_state;

   public:
    Sim(/* args */) {
        bin_state = std::vector<std::vector<int>>(BIN_WIDTH, std::vector<int>(BIN_LENGTH, 0));
    }
    ~Sim() {
    }
    int update_state(std::pair<int, int> start_corner, int width, int length, int height) {
        int cur_max_height = -1;
        int scaled_length = length * SCALING_FACTOR;
        int scaled_width = width * SCALING_FACTOR;
        int scaled_height = height * SCALING_FACTOR;
        try {
            for (int start_x = start_corner.first; start_x <= start_corner.first + scaled_width - 1; start_x++) {
                for (int start_y = start_corner.second; start_y <= start_corner.second + scaled_length - 1; start_y++) {
                    bin_state[start_x][start_y] += scaled_height;
                    cur_max_height = std::max(cur_max_height, bin_state[start_x][start_y]);
                }
            }
            for (int start_x = start_corner.first; start_x <= start_corner.first + scaled_width - 1; start_x++) {
                for (int start_y = start_corner.second; start_y <= start_corner.second + scaled_length - 1; start_y++) {
                    bin_state[start_x][start_y] = cur_max_height;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return 0;
        }

        return 1;
    }
    void print_state() {
        for (int i = 0; i < BIN_WIDTH; i++) {
            for (int j = 0; j < BIN_LENGTH; j++) {
                std::cout << bin_state[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
    std::vector<std::vector<int>> get_state() {
        return bin_state;
    }
};

// int main() {
//     Sim sim = Sim();
//     if (sim.update_state({0, 0}, 5, 5, 10)) {
//         sim.print_state();
//     }
//     return 0;
// }
