#include <algorithm>

#include "base.h"

class Random_Algorithm : public Base {
   private:
   std::vector<double>params;
    int find_holes(std::vector<std::vector<int>> cur_state, std::pair<int, int> xy, vector_3d dim) {
        int max_height = grid_max(cur_state, xy.first, xy.first + dim.x, xy.second, xy.second + dim.y);
        int holes = 0;
        for (int i = xy.first; i < xy.first + dim.x; i++) {
            for (int j = xy.second; j < xy.second + dim.y; j++) {
                holes += max_height - cur_state[i][j];
            }
        }
        return holes;
    }
    std::pair<int, int> get_action(Bin &cur_bin, vector_3d &dim) {
        auto icpbcp_list = cur_bin.get_icbp_list();
        auto cur_state = cur_bin.get_state();
        int lx = dim.x, ly = dim.y, lz = dim.z;
        double max_score = INT_MIN;
        int idx = -1;
        int ori = -1;
        std::vector<std::pair<int,int>>to_be_selected;
        for (int i = 0; i < icpbcp_list.size(); i++) {
            if (check_without_precomputation(cur_state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim)) {
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;

                if ((x_diff >= lx && y_diff >= ly && z_diff >= lz)) {
                    // int holes = find_holes(cur_state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim);
                    // Bin temp_bin = cur_bin;
                    // temp_bin.update_state({icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim);
                    // eval_feature x;
                    // x.holes = holes;
                    // std::vector<double>features = extract_state_features(temp_bin,x);
                    // double temp_max = evaluate(features);
                    // if(temp_max > max_score){
                    //     idx = i;
                    //     max_score = temp_max;
                    //     ori = 0;
                    // }
                    to_be_selected.push_back({i,0});
                }
            }
            if (check_without_precomputation(cur_state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, {dim.y, dim.x, dim.z})) {
                // int holes = find_holes(cur_state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim);
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;
                if ((x_diff >= lx && y_diff >= ly && z_diff >= lz)) {
                    // int holes = find_holes(cur_state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim);
                    // Bin temp_bin = cur_bin;
                    // temp_bin.update_state({icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim);
                    // eval_feature x;
                    // x.holes=holes;
                    // std::vector<double> features = extract_state_features(temp_bin,x);
                    // double temp_max = evaluate(features);
                    // if (temp_max > max_score) {
                    //     idx = i;
                    //     max_score = temp_max;
                    //     ori = 1;
                    // }
                    to_be_selected.push_back({i, 1});
                }
            }
        }
        if(to_be_selected.size()==0){
            return {-1,-1};
        }
        int selected = dist(generator)%to_be_selected.size();
        return to_be_selected[selected];
    }

   public:
    std::mt19937 generator;
    std::uniform_int_distribution<int> dist;
    Random_Algorithm() {
        generator=std::mt19937(time(0));
        dist=std::uniform_int_distribution<int>(0, 100000);
    }
    Random_Algorithm(GenerateBox gb, Sim &simulator) : Base(gb, simulator) {
        this->params = params;
    }
    bool put_box(Sim &simulator, int bin_id, vector_3d box) {
        std::vector<std::vector<int>> cur_state = simulator.bin_instances[bin_id].get_state();
        // bin_instance.print_state();
        std::vector<std::pair<vector_3d, vector_3d>> icpbcp_list = simulator.bin_instances[bin_id].get_icbp_list();
        //precompute_max_min(cur_state);
        auto idx_ori = get_action(simulator.bin_instances[bin_id], box);

        // std::cout << idx_ori.first << "\n";
        if (idx_ori.first >= 0) {
            return simulator.step(bin_id, idx_ori.first, box, idx_ori.second);
        } else {
            //std::cout << "could not place the box" << std::endl;
        }
        return 0;
    }
    int evaluate(std::vector<double> &features) {
        double sum = 0;
        for (int i = 0; i < features.size(); i++) {
            sum += features[i] * params[i];
        }
        // std::cout<<sum<<"\n";
        return sum;
    }

    void extract_pool_features(std::vector<std::vector<int>>&after_state,eval_feature  &x,int stride,int filter_size){
        for (int i = 0; i < BIN_WIDTH / stride; i++) {
            for (int j = 0; j < BIN_LENGTH / stride; j++) {
                int temp_max = 0;
                int temp_min = 0;
                double mean_height = 0;
                for (int inner_cell_x = i * stride; inner_cell_x < i * stride + filter_size; inner_cell_x++) {
                    for (int inner_cell_y = j * stride; inner_cell_y < j * stride + filter_size; inner_cell_y++) {
                        temp_max = std::max(temp_max, after_state[inner_cell_x][inner_cell_y]);
                        temp_min = std::min(temp_min, after_state[inner_cell_x][inner_cell_y]);
                        mean_height += after_state[inner_cell_x][inner_cell_y];
                    }
                }
                x.max_pool.push_back(temp_max);
                x.min_pool.push_back(temp_min);
                x.avg_pool.push_back(mean_height / (filter_size * filter_size));
            }
        }
    }

    std::vector<double> extract_state_features(Bin cur_bin,eval_feature &x) {
        auto cur_state = cur_bin.get_state();
        std::vector<double>features;
        extract_pool_features(cur_state,x,STRIDE,FILTER_SIZE);
        for(int i:x.max_pool){
            features.push_back(i);
        }
        for (int i : x.min_pool) {
            features.push_back(i);
        }
        for (int i : x.avg_pool) {
            features.push_back(i);
        }
        features.push_back(x.holes);
        return features;
    }

    performance_metric execute(Sim &simulator, int lookahead) {
        simulator.size_of_box_stream=boxes.size();
        for (auto box : boxes) {
            // std::cout << box.x << " " << box.y << " " << box.z << "\n";
            int flag = 0;
            for (int i = 0; i < simulator.bin_instances.size(); i++) {
                if (simulator.bin_instances[i].is_open()) {
                    if (put_box(simulator, i, box)) {
                        // std::cout<<"put"<<"\n";
                        flag = 1;
                        break;
                    }
                }
            }
            if (!flag && simulator.open_new_bin()) {
                put_box(simulator, simulator.bin_instances.size() - 1, box);
            }
        }

        return simulator.get_performance_metric(1);
    }
};