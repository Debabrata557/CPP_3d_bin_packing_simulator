#include <random>
#include <tuple>
#include <iostream>
#ifndef config
#define config
const int DEBUG = 0;
const int BIN_WIDTH = 120;
const int BIN_HEIGHT = 120;
const int BIN_LENGTH = 180;
const int MAX_BOX_LENGTH = 60;
const int MAX_BOX_WIDTH = 60;
const int MAX_BOX_HEIGHT = 60;
const int MIN_BOX_LENGTH = 30;
const int MIN_BOX_WIDTH = 30;
const int MIN_BOX_HEIGHT = 30;
const int CONTROLLER_TOLERANCE = 3;

const int SCALING_FACTOR = 1; // effective bin_width/height/length = bin_width/height/length *(1/scaling_factor)..Make sure the effective bin_width/height/length are integers

typedef struct vector_3d
{
    int x, y, z;
    // vector_3d() {
    //     x = 0, y = 0, z = 0;
    // }
    // vector_3d(int x, int y, int z) {
    //     this->x = x;
    //     this->y = y;
    //     this->z = z;
    // }
} vector_3d;

typedef struct performance_metric
{
    double number_of_boxes_successfully_put;
    double total_number_of_boxes;
    double efficiency;
    double number_of_bins_used;
    bool exception;
} performance_metric;
bool operator<(const vector_3d &a, const vector_3d &b);
bool comp_floor_building(std::pair<vector_3d, vector_3d> &a, std::pair<vector_3d, vector_3d> &b);
void print_vector_3d(vector_3d &a);
void print_list_vector_3d(std::vector<vector_3d> &a);
void print_pair_vector_3d(std::pair<vector_3d, vector_3d> &a);
void print_list_pair_vector_3d(std::vector<std::pair<vector_3d, vector_3d>> &a);
#endif
