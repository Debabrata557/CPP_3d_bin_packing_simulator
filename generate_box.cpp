#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "config.h"

class GenerateBox {
   private:
    std::vector<int> max_size, min_size;
    std::vector<std::vector<int>> stream_of_boxes;
    // int scaled_max_box_length, scaled_max_box_width, scaled_max_box_height;
    // int scaled_min_box_length, scaled_min_box_width, scaled_min_box_height;

    /* data */
   public:
    GenerateBox(/* args */) {
        max_size = {MAX_BOX_WIDTH, MAX_BOX_LENGTH, MAX_BOX_HEIGHT};
        min_size = {MIN_BOX_WIDTH, MIN_BOX_LENGTH, MIN_BOX_HEIGHT};
        // scaled_max_box_length = MAX_BOX_LENGTH * SCALING_FACTOR;
        // scaled_max_box_height = MAX_BOX_HEIGHT * SCALING_FACTOR;
        // scaled_max_box_width = MAX_BOX_WIDTH * SCALING_FACTOR;
        // scaled_min_box_length = MIN_BOX_LENGTH * SCALING_FACTOR;
        // scaled_min_box_height = MIN_BOX_HEIGHT * SCALING_FACTOR;
        // scaled_min_box_width = MIN_BOX_WIDTH * SCALING_FACTOR;
    }
    ~GenerateBox() {
    }

    bool is_valid_item(std::vector<int>& item) {
        for (int i = 0; i < 3; i++) {
            if (item[i] < min_size[i] || item[i] > max_size[i])
                return false;
        }
        return true;
    }
    int generate_cut1() {
        try {
            std::vector<std::vector<int>> invalid;
            invalid.push_back(std::vector<int>({BIN_WIDTH, BIN_LENGTH, BIN_HEIGHT, 0, 0, 0}));
            while (invalid.size() > 0) {
                int invalidItemIndex = rand() % invalid.size();
                std::vector<int> invalidItem = invalid[invalidItemIndex];
                invalid.erase(invalid.begin() + invalidItemIndex);
                std::vector<int> axis;
                if (invalidItem[0] > max_size[0])
                    axis.push_back(0);
                if (invalidItem[1] > max_size[1])
                    axis.push_back(1);
                if (invalidItem[2] > max_size[2])
                    axis.push_back(2);
                int axisChosen = axis[rand() % axis.size()];
                int cutLen = rand() % (invalidItem[axisChosen] - 2 * min_size[axisChosen]);
                std::vector<int> item1 = {invalidItem[0], invalidItem[1], invalidItem[2], invalidItem[3], invalidItem[4], invalidItem[5]};
                std::vector<int> item2 = {invalidItem[0], invalidItem[1], invalidItem[2], invalidItem[3], invalidItem[4], invalidItem[5]};
                item1[axisChosen] = min_size[axisChosen] + cutLen;
                item2[axisChosen] = invalidItem[axisChosen] - item1[axisChosen];
                item2[3 + axisChosen] = item2[3 + axisChosen] + item1[axisChosen];
                if (is_valid_item(item1))
                    stream_of_boxes.push_back(item1);
                else
                    invalid.push_back(item1);
                if (is_valid_item(item2))
                    stream_of_boxes.push_back(item2);
                else
                    invalid.push_back(item2);
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            return 0;
        }

        return 1;
    }

    void generate_cut2() {
    }
    std::vector<std::vector<int>> get_stream_of_boxes() {
        return stream_of_boxes;
    }
};

// int main() {
//     GenerateBox gb = GenerateBox();
//     std::vector<std::vector<int>> boxes;
//     if (gb.generate_cut1()) {
//         boxes = gb.get_stream_of_boxes();
//     }
//     assert(boxes.size() != 0);
//     std::cout << "box_generation_succesful"
//               << " " << boxes.size()
//               << "\n";

//     return 0;
// }
