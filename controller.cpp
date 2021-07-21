#include <vector>

#include "config.h"
#include "helper.h"

class Controller {
   private:
    /* data */
   public:
    Controller(/* args */);
    ~Controller();
    bool check(const std::vector<std::vector<int>> &state, std::pair<int, int> pos, int lx, int ly, int lz);
    std::vector<int> first_fit(std::vector<std::vector<int>> &state, std::vector<int> &dim);
};

Controller::Controller(/* args */) {
}

Controller::~Controller() {
}

bool Controller::check(const std::vector<std::vector<int>> &state, std::pair<int, int> pos, int lx, int ly, int lz) {
    int minx = pos.first;
    int maxx = pos.first + lx - 1;
    int miny = pos.second;
    int maxy = pos.second + ly - 1;
    int surface = 5;

    if (minx >= 0 && miny >= 0 && maxx <= BIN_WIDTH - 1 && maxy <= BIN_LENGTH - 1) {
        // mat=state[minx:maxx+1,miny:maxy+1]
        int max_height = grid_max(state, minx, maxx + 1, miny, maxy + 1);
        int corner1_max = grid_max(state, minx, minx + surface + 1, miny, miny + surface + 1);
        int corner2_max = grid_max(state, maxx - surface, maxx + 1, miny, miny + surface + 1);
        int corner3_max = grid_max(state, minx, minx + surface + 1, maxy, maxy - surface + 1);
        int corner4_max = grid_max(state, maxx - surface, maxx + 1, maxy - surface, maxy + 1);
        int corner1_min = grid_min(state, minx, minx + surface + 1, miny, miny + surface + 1);
        int corner2_min = grid_min(state, maxx - surface, maxx + 1, miny, miny + surface + 1);
        int corner3_min = grid_min(state, minx, minx + surface + 1, maxy, maxy - surface + 1);
        int corner4_min = grid_min(state, maxx - surface, maxx + 1, maxy - surface, maxy + 1);

        // corner1_mat=state[minx:minx+surface+1,miny:miny+surface+1]
        // corner2_mat=state[maxx-surface:maxx+1,miny:miny+surface+1]
        // corner3_mat=state[minx:minx+surface+1,maxy-surface:maxy+1]
        // corner4_mat=state[maxx-surface:maxx+1,maxy-surface:maxy+1]
        // # print(mat.shape)
        // h=np.max(mat)
        // #print('max h: ',h)
        if (max_height + lz >= BIN_HEIGHT) {
            return 0;
        }

        // tolerance=1
        int support = grid_count(state, minx, maxx + 1, miny, maxy + 1, max_height, CONTROLLER_TOLERANCE);
        int corner_support = 0;
        corner_support += (abs(corner1_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner1_min - max_height) <= CONTROLLER_TOLERANCE);
        corner_support += (abs(corner2_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner2_min - max_height) <= CONTROLLER_TOLERANCE);
        corner_support += (abs(corner3_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner3_min - max_height) <= CONTROLLER_TOLERANCE);
        corner_support += (abs(corner4_max - max_height) <= CONTROLLER_TOLERANCE) && (abs(corner4_min - max_height) <= CONTROLLER_TOLERANCE);
        if (corner_support == 4)
            return support >= 0.6;
        if (corner_support == 3)
            return support >= 0.8;
        return support >= 0.95;
    }
    return 0;
}

std::vector<int> Controller::first_fit(std::vector<std::vector<int>> &state, std::vector<int> &dim) {
    int lx = dim[0], ly = dim[1], lz = dim[2];
    // #print('checking new dim: ', lx, ',', ly, ',', lz)
    // #bin_packing_helper.printStates(state)

    for (int i = 0; i < BIN_WIDTH; i++) {
        for (int j = 0; j < BIN_LENGTH; j++) {
            int flag = Controller::check(state, {i, j}, lx, ly, lz);
            if (flag)
                return {i, j, 0};
            flag = Controller::check(state, {j, i}, lx, ly, lz);
            if (flag)
                return {i, j, 1};
        }
    }
    return {-40, -40, 0};
}

// int main() {
//     Controller controller = Controller();
//     return 0;
// }
