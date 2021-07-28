#include "config.h"

// struct vector_3d {
//     int x, y, z;
//     // vector_3d() {
//     //     x = 0, y = 0, z = 0;
//     // }
//     // vector_3d(int x, int y, int z) {
//     //     this->x = x;
//     //     this->y = y;
//     //     this->z = z;
//     // }
// };

bool operator<(const vector_3d& a, const vector_3d& b) {
    return std::tie(a.z, a.x, a.y) < std::tie(a.z, b.x, b.y);
}
