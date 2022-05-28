#include "base.h"
// #include "config.h"
// #include "helper.h"

#include <algorithm>

/*

Take features from the complete state.
*/

class Walle {
   public:
    Walle() {
    }
    static std::vector<int> calculate_walle_score(std::vector<std::vector<int>> &cur_state, std::vector<int> pos, int lx, int ly, int lz) {
        int minx = pos[0], maxx = pos[0] + lx - 1, miny = pos[1], maxy = pos[1] + ly - 1;
        std::vector<int> gvar(4, 0);
        std::vector<int> ghigh{ly, lx, ly, lx};
        std::vector<int> gflush(4, 0);
        int margin = 2;
        int temp1 = 0, temp2 = 0;
        if (minx - margin > 0) {
            for (int j = miny; j <= maxy; j++) {
                gvar[0] += abs(cur_state[minx - 1 - margin][j] - cur_state[minx + 1][j] - lz);
                temp1 += abs(cur_state[minx - 1 - margin][j] - cur_state[minx + 1][j] - lz) > CONTROLLER_TOLERANCE;
                temp2 += abs(cur_state[minx - 1 - margin][j] - cur_state[minx + 1][j] - lz) <= CONTROLLER_TOLERANCE;
            }
        }
        ghigh[0] = temp1;
        gflush[0] = temp2;
        temp1 = 0;
        temp2 = 0;
        if (miny - margin > 0) {
            for (int i = minx; i <= maxx; i++) {
                gvar[1] += abs(cur_state[i][miny - margin - 1] - cur_state[i][miny + 1] - lz);
                temp1 += abs(cur_state[i][miny - margin - 1] - cur_state[i][miny + 1] - lz) > CONTROLLER_TOLERANCE;
                temp2 += abs(cur_state[i][miny - margin - 1] - cur_state[i][miny + 1] - lz) <= CONTROLLER_TOLERANCE;
            }
        }
        ghigh[1] = temp1;
        gflush[1] = temp2;
        temp1 = 0;
        temp2 = 0;
        if (maxx + margin < BIN_WIDTH - 1) {
            for (int j = miny; j <= maxy; j++) {
                gvar[2] += abs(cur_state[maxx + 1 + margin][j] - cur_state[maxx - 1][j] - lz);
                temp1 += abs(cur_state[maxx + 1 + margin][j] - cur_state[maxx - 1][j] - lz) > CONTROLLER_TOLERANCE;
                temp2 += abs(cur_state[maxx + 1 + margin][j] - cur_state[maxx - 1][j] - lz) <= CONTROLLER_TOLERANCE;
            }
        }
        ghigh[2] = temp1;
        gflush[2] = temp2;
        temp1 = 0;
        temp2 = 0;
        if (maxy + margin < BIN_LENGTH - 1) {
            for (int i = minx; i <= maxx; i++) {
                gvar[3] += abs(cur_state[i][maxy + 1 + margin] - cur_state[i][maxy - 1] - lz);
                temp1 += abs(cur_state[i][maxy + 1 + margin] - cur_state[i][maxy - 1] - lz) > CONTROLLER_TOLERANCE;
                temp2 += abs(cur_state[i][maxy + 1 + margin] - cur_state[i][maxy - 1] - lz) <= CONTROLLER_TOLERANCE;
            }
        }
        ghigh[3] = temp1;
        gflush[3] = temp2;
        return {std::accumulate(gvar.begin(), gvar.end(), 0), std::accumulate(gflush.begin(), gflush.end(), 0), std::accumulate(ghigh.begin(), ghigh.end(), 0)};
    }
};