#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "config.h"

#ifndef generate_box
#define generate_box

class GenerateBox
{
private:
    std::vector<int> max_size, min_size;
    std::vector<vector_3d> stream_of_boxes;
    std::mt19937 generator;
    std::uniform_int_distribution<int> cpp_rand;
    // int scaled_max_box_length, scaled_max_box_width, scaled_max_box_height;
    // int scaled_min_box_length, scaled_min_box_width, scaled_min_box_height;

    /* data */
public:
    GenerateBox(/* args */);
    GenerateBox(int seed, std::string algorithm, int number = 0);
    ~GenerateBox();

    bool is_valid_item(std::vector<int> &item);
    int generate_cut1(int repeat);
    void generate_cut2();
    int generate_random(int number);
    std::vector<vector_3d> get_stream_of_boxes();
};

#endif

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
