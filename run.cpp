#include "controller.cpp"
#include "generate_box.cpp"
#include "sim.cpp"

int main() {
    GenerateBox gb = GenerateBox();
    Sim sim = Sim();
    Controller controller = Controller();
    std::vector<std::vector<int>> boxes;
    if (gb.generate_cut1()) {
        boxes = gb.get_stream_of_boxes();
    }
    long long sum = 0;
    for (auto box : boxes) {
        std::vector<std::vector<int>> cur_state = sim.get_state();
        std::vector<int> pose = controller.first_fit(cur_state, box);
        if (pose[0] >= 0) {
            if (pose[2] == 0) {
                if (sim.update_state({pose[0], pose[1]}, box[0], box[1], box[2])) {
                    sum += box[0] * box[1] * box[2];
                }
            } else {
                if (sim.update_state({pose[0], pose[1]}, box[1], box[0], box[2])) {
                    sum += box[0] * box[1] * box[2];
                }
            }
        } else {
            std::cout << "could not place the box" << std::endl;
        }
    }
    return 0;
}