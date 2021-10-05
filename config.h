#include <iostream>
#include <random>
#include <tuple>
#ifndef config
#define config
const int DEBUG = 0;
const int BIN_WIDTH = 120;  //x
const int BIN_HEIGHT = 120;
const int BIN_LENGTH = 180;  //y
const int MAX_BOX_LENGTH = 60;
const int MAX_BOX_WIDTH = 60;
const int MAX_BOX_HEIGHT = 60;
const int MIN_BOX_LENGTH = 30;
const int MIN_BOX_WIDTH = 30;
const int MIN_BOX_HEIGHT = 30;
const int CONTROLLER_TOLERANCE = 3;

const int SCALING_FACTOR = 1;  // effective bin_width/height/length = bin_width/height/length *(1/scaling_factor)..Make sure the effective bin_width/height/length are integers

const int FILTER_SIZE = 20;
const int STRIDE = 20;
const int EXTRACT_FEATURE_AREA = 70;
const int NUM_FEATURES = 80;

//const int NPARAMS_X = (BIN_WIDTH+STRIDE-1)/STRIDE; //without symmetry
//const int NPARAMS_Y = (BIN_LENGTH+STRIDE-1)/STRIDE;

const int NPARAMS_X = ((BIN_WIDTH / 2) + STRIDE - 1) / STRIDE;  // with symmetry
const int NPARAMS_Y = ((BIN_LENGTH / 2) + STRIDE - 1) / STRIDE;

const int POOL_PARAMS = 3 * NPARAMS_X * NPARAMS_Y;

const int BOUNDARY_STRIDE = 10;
const int BOUNDARY_PARAMS = MAX_BOX_LENGTH * 2 / BOUNDARY_STRIDE + 2 * MAX_BOX_WIDTH / BOUNDARY_STRIDE;
const int BIAS_HOLE = 2;
const int TOTAL_PARAMS = POOL_PARAMS + BOUNDARY_PARAMS + BIAS_HOLE;

typedef struct vector_3d {
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

typedef struct performance_metric {
    double number_of_boxes_successfully_put;
    double total_number_of_boxes;
    double efficiency;
    double number_of_bins_used;
    bool exception;
} performance_metric;

typedef struct eval_feature {
    std::vector<double> max_pool;
    std::vector<double> min_pool;
    std::vector<double> avg_pool;
    std::vector<double> border_features;
    double border_diff_height;
    double holes;
} eval_feature;
bool operator<(const vector_3d &a, const vector_3d &b);
bool comp_floor_building(std::pair<vector_3d, vector_3d> &a, std::pair<vector_3d, vector_3d> &b);
bool comp_first_fit(std::pair<vector_3d, vector_3d> &a, std::pair<vector_3d, vector_3d> &b);
void print_vector_3d(vector_3d &a);
void print_list_vector_3d(std::vector<vector_3d> &a);
void print_pair_vector_3d(std::pair<vector_3d, vector_3d> &a);
void print_list_pair_vector_3d(std::vector<std::pair<vector_3d, vector_3d>> &a);
void print_state(std::vector<std::vector<int>> &a);
#define DEBUG_ENABLED 1
#if DEBUG_ENABLED
#define DEBUG(a) cout << a
#else
#define DEBUG(a) (void)0
#endif
#endif
