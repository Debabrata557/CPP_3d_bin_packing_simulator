
/*

Take features from the complete state.
*/

#include <algorithm>
#include <fstream>

#include "base.h"

class Smart_Algorithm : public Base {
   private:
    std::ofstream write_file;
    std::string write_file_name = "details.txt";
    std::vector<double> params;
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
    vector_3d get_action(Sim &simulator, vector_3d &dim) {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        double max_score = -DBL_MAX;
        int idx = -1;
        int ori = -1;
        int bin_id=-1;
        for(int s=0;s<simulator.bin_instances.size(); s++){
            auto cur_bin=simulator.bin_instances[s];
            if(!cur_bin.is_open())continue;
            auto icpbcp_list = cur_bin.get_icbp_list();
            auto cur_state = cur_bin.get_state();
            for (int i = 0; i < icpbcp_list.size(); i++) {
                if (check_without_precomputation(cur_state, icpbcp_list[i], dim)) {
                    auto icp_bcp = icpbcp_list[i];
                    auto start_point =get_start_point(icp_bcp, 0, dim);
                    int holes = find_holes(cur_state, start_point, dim);
                    Bin temp_bin = cur_bin;
                    temp_bin.update_state(start_point, dim);
                    temp_bin.volume+=dim.x*dim.y*dim.z;
                    eval_feature x;
                    x.holes = holes;
                    std::vector<double> features = extract_state_features_with_symmetry(temp_bin, x, dim, start_point.first, start_point.second);
                    // double temp_max = evaluate(features);
                    double temp_max = evaluate_with_symmetry(features);
                    if (temp_max > max_score) {
                        idx = i;
                        max_score = temp_max;
                        ori = 0;
                        bin_id=s;
                    }
                }
                vector_3d rotated_dim = {dim.y, dim.x, dim.z};
                lx = rotated_dim.x, ly = rotated_dim.y, lz = rotated_dim.z;
                if (check_without_precomputation(cur_state, icpbcp_list[i], rotated_dim)) {
                    auto icp_bcp = icpbcp_list[i];
                    auto start_point =get_start_point(icp_bcp, 1, dim);
                    int holes = find_holes(cur_state, start_point, rotated_dim);
                    Bin temp_bin = cur_bin;
                    temp_bin.update_state(start_point, rotated_dim);
                    temp_bin.volume+=rotated_dim.x*rotated_dim.y*rotated_dim.z;
                    eval_feature x;
                    x.holes = holes;
                    std::vector<double> features = extract_state_features_with_symmetry(temp_bin, x, rotated_dim, start_point.first, start_point.second);
                    // double temp_max = evaluate(features);
                    double temp_max = evaluate_with_symmetry(features);
                    if (temp_max > max_score) {
                        idx = i;
                        max_score = temp_max;
                        ori = 1;
                        bin_id=s;
                    }
                }
            }
        }
        
        return {idx, ori, bin_id};
    }

   public:
    Smart_Algorithm() {
    }
    Smart_Algorithm(GenerateBox gb, Sim &simulator, const std::vector<double> &params) : Base(gb, simulator) {
        this->params = params;
    }
    bool put_box(Sim &simulator, vector_3d box) {
        //std::vector<std::vector<int>> cur_state = simulator.bin_instances[bin_id].get_state();
        // bin_instance.print_state();
        //precompute_max_min(cur_state);
        auto idx_ori_bin = get_action(simulator, box);
        if(idx_ori_bin.z==-1)return 0;
        auto icp_bcp = simulator.bin_instances[idx_ori_bin.z].get_icbp_list()[idx_ori_bin.x];
        // std::cout << idx_ori.first << "\n";
        if (idx_ori_bin.x >= 0) {
            int height = simulator.step(idx_ori_bin.z, idx_ori_bin.x, box, idx_ori_bin.y);
            auto start_point=get_start_point(icp_bcp, idx_ori_bin.y, box);
            write_file << idx_ori_bin.z << " " << box.x << " " << box.y << " " << box.z << " " << start_point.first << " " << start_point.second << " " << height << " " << idx_ori_bin.y << "\n";
            return height != -1;
        } else {
            write_file << idx_ori_bin.z << " " << box.x << " " << box.y << " " << box.z << " " << icp_bcp.x << " " << icp_bcp.y << " " << -1 << " " << idx_ori_bin.y << "\n";
            //std::cout << "could not place the box" << std::endl;
        }
        return 0;
    }
    int evaluate(std::vector<double> &features) {
        double sum = 0;
        //assert params.size()==features.size();
        for (int i = 0; i < features.size(); i++) {
            sum += features[i] * params[i];
        }
        // std::cout<<sum<<"\n";
        return sum;
    }
    double evaluate_with_symmetry(std::vector<double> &features) {
        double sum = 0;
        //assert params.size()==features.size();
        for(int i=0;i<BIAS_HOLE;i++){
            sum+=params[i]*features[i];
        }
        int j = BIAS_HOLE;

        for (int i = BIAS_HOLE; i < POOL_PARAMS + BIAS_HOLE; i++, j++) {
            sum += features[j] * params[i];
            assert(j<features.size());
        }
        for (int i = BIAS_HOLE; i < POOL_PARAMS + BIAS_HOLE; i++, j++) {
            sum += features[j] * params[i];
            assert(j<features.size());
        }
        for (int i = BIAS_HOLE; i < POOL_PARAMS + BIAS_HOLE; i++, j++) {
            sum += features[j] * params[i];
            assert(j<features.size());
        }
        for (int i = BIAS_HOLE; i < POOL_PARAMS + BIAS_HOLE; i++, j++) {
            sum += features[j] * params[i];
            assert(j<features.size());
        }
        for (int i = BIAS_HOLE + POOL_PARAMS; i < BOUNDARY_PARAMS + BIAS_HOLE + POOL_PARAMS; i++, j++) {
            sum += features[j] * params[i];
            //std::cout<<j<<" "<<features.size()<<std::endl;
            assert(j<features.size());
        }
        // std::cout<<sum<<"\n";
        return sum;
    }

    void extract_pool_features(std::vector<std::vector<int>> &after_state, int start_x, int end_x, int start_y, int end_y, eval_feature &x, int stride, int filter_size) {
        for (int i = start_x; i < end_x; i += stride) {
            for (int j = start_y; j < end_y; j += stride) {
                int temp_max = 0;
                int temp_min = 0;
                double mean_height = 0;
                int cell_count = 0;
                for (int inner_i = i; inner_i <= std::min(i + filter_size - 1, end_x); inner_i++) {
                    for (int inner_j = j; inner_j <= std::min(j + filter_size - 1, end_y); inner_j++) {
                        temp_max = std::max(temp_max, after_state[inner_i][inner_j]);
                        temp_min = std::min(temp_min, after_state[inner_i][inner_j]);
                        mean_height += after_state[inner_i][inner_j];
                        cell_count++;
                    }
                }
                x.max_pool.push_back(temp_max);
                x.min_pool.push_back(temp_min);
                x.avg_pool.push_back(mean_height / cell_count);
            }
        }
    }

    void extract_pool_features_with_symmetry(std::vector<std::vector<int>> &after_state, int start_x, int end_x, int start_y, int end_y, eval_feature &x, int stride, int filter_size) {
        for (int i = start_x; i < end_x / 2; i += stride) {
            for (int j = start_y; j < end_y / 2; j += stride) {
                int temp_max = 0;
                int temp_min = 0;
                double mean_height = 0;
                int cell_count = 0;
                for (int inner_i = i; inner_i <= std::min(i + filter_size - 1, end_x / 2); inner_i++) {
                    for (int inner_j = j; inner_j <= std::min(j + filter_size - 1, end_y / 2); inner_j++) {
                        temp_max = std::max(temp_max, after_state[inner_i][inner_j]);
                        temp_min = std::min(temp_min, after_state[inner_i][inner_j]);
                        mean_height += after_state[inner_i][inner_j];
                        cell_count++;
                    }
                }
                x.max_pool.push_back(temp_max);
                x.min_pool.push_back(temp_min);
                x.avg_pool.push_back(mean_height / cell_count);
            }
        }
        for (int i = start_x; i < end_x / 2; i += stride) {
            for (int j = end_y - 1; j >= end_y / 2; j -= stride) {
                int temp_max = 0;
                int temp_min = 0;
                double mean_height = 0;
                int cell_count = 0;
                for (int inner_i = i; inner_i <= std::min(i + filter_size - 1, end_x / 2); inner_i++) {
                    for (int inner_j = j; inner_j >= std::max(j - filter_size + 1, end_y / 2); inner_j--) {
                        temp_max = std::max(temp_max, after_state[inner_i][inner_j]);
                        temp_min = std::min(temp_min, after_state[inner_i][inner_j]);
                        mean_height += after_state[inner_i][inner_j];
                        cell_count++;
                    }
                }
                x.max_pool.push_back(temp_max);
                x.min_pool.push_back(temp_min);
                x.avg_pool.push_back(mean_height / cell_count);
            }
        }
        for (int i = end_x - 1; i >= end_x / 2; i -= stride) {
            for (int j = start_y; j < end_y / 2; j += stride) {
                int temp_max = 0;
                int temp_min = 0;
                double mean_height = 0;
                int cell_count = 0;
                for (int inner_i = i; inner_i >= std::max(i - filter_size + 1, end_x / 2); inner_i--) {
                    for (int inner_j = j; inner_j <= std::min(j + filter_size - 1, end_y / 2); inner_j++) {
                        temp_max = std::max(temp_max, after_state[inner_i][inner_j]);
                        temp_min = std::min(temp_min, after_state[inner_i][inner_j]);
                        mean_height += after_state[inner_i][inner_j];
                        cell_count++;
                    }
                }
                x.max_pool.push_back(temp_max);
                x.min_pool.push_back(temp_min);
                x.avg_pool.push_back(mean_height / cell_count);
            }
        }
        for (int i = end_x - 1; i >= end_x / 2; i -= stride) {
            for (int j = end_y - 1; j >= end_y / 2; j -= stride) {
                int temp_max = 0;
                int temp_min = 0;
                double mean_height = 0;
                int cell_count = 0;
                for (int inner_i = i; inner_i >= std::max(i - filter_size + 1, end_x / 2); inner_i--) {
                    for (int inner_j = j; inner_j >= std::max(j - filter_size + 1, end_y / 2); inner_j--) {
                        temp_max = std::max(temp_max, after_state[inner_i][inner_j]);
                        temp_min = std::min(temp_min, after_state[inner_i][inner_j]);
                        mean_height += after_state[inner_i][inner_j];
                        cell_count++;
                    }
                }
                x.max_pool.push_back(temp_max);
                x.min_pool.push_back(temp_min);
                x.avg_pool.push_back(mean_height / cell_count);
            }
        }
    }

    void extract_border_feature(std::vector<std::vector<int>> &after_state, vector_3d &dim, int pos_x, int pos_y, eval_feature &x) {
        int sum = 0;
        for (int i = pos_x; i < pos_x + dim.x; i += BOUNDARY_STRIDE) {
            if (pos_y > 0) {
                x.border_features.push_back(abs(after_state[i][pos_y] - after_state[i][pos_y - 1]));
            } else {
                x.border_features.push_back(0);
            }
            if (pos_y + dim.y < BIN_LENGTH) {
                x.border_features.push_back(abs(after_state[i][pos_y + dim.y] - after_state[i][pos_y + dim.y - 1]));
            } else {
                x.border_features.push_back(0);
            }
        }
        for (int i = pos_y; i < pos_y + dim.y; i += BOUNDARY_STRIDE) {
            if (pos_x > 0) {
                x.border_features.push_back(abs(after_state[pos_x][i] - after_state[pos_x - 1][i]));
            } else {
                x.border_features.push_back(0);
            }
            if (pos_x + dim.x < BIN_WIDTH) {
                x.border_features.push_back(abs(after_state[pos_x + dim.x][i] - after_state[pos_x + dim.x - 1][i]));
            } else {
                x.border_features.push_back(0);
            }
        }
    }
    std::vector<double> extract_state_features(Bin cur_bin, eval_feature &x, vector_3d &dim, int pos_x, int pos_y) {
        auto cur_state = cur_bin.get_state();
        std::vector<double> features;
        int start_x = 0;
        int start_y = 0;
        int end_x = BIN_WIDTH;
        int end_y = BIN_LENGTH;
        // extract_pool_features_with_symmetry(cur_state,start_x,end_x,start_y,end_y,x,STRIDE,FILTER_SIZE);
        extract_pool_features(cur_state, start_x, end_x, start_y, end_y, x, STRIDE, FILTER_SIZE);

        extract_border_feature(cur_state, dim, pos_x, pos_y, x);
        features.push_back(1);
        features.push_back(x.holes / (0.5 * BIN_HEIGHT * BIN_LENGTH * BIN_WIDTH));
        for (int i : x.max_pool) {
            features.push_back(i * 1.0 / BIN_HEIGHT);
        }
        for (int i : x.min_pool) {
            features.push_back(i * 1.0 / BIN_HEIGHT);
        }
        for (int i : x.avg_pool) {
            features.push_back(i * 1.0 / BIN_HEIGHT);
        }
        for (int i : x.border_features) {
            features.push_back(i * 1.0 / BIN_HEIGHT);
        }
        return features;
    }

    std::vector<double> extract_state_features_with_symmetry(Bin cur_bin, eval_feature &x, vector_3d &dim, int pos_x, int pos_y) {
        auto cur_state = cur_bin.get_state();
        std::vector<double> features;
        int start_x = 0;
        int start_y = 0;
        int end_x = BIN_WIDTH;
        int end_y = BIN_LENGTH;
        extract_pool_features_with_symmetry(cur_state, start_x, end_x, start_y, end_y, x, STRIDE, FILTER_SIZE);
        // extract_pool_features(cur_state, start_x, end_x, start_y, end_y, x, STRIDE, FILTER_SIZE);

        extract_border_feature(cur_state, dim, pos_x, pos_y, x);
        features.push_back(1);
        features.push_back(x.holes / (0.5 * BIN_HEIGHT * BIN_LENGTH * BIN_WIDTH));
        features.push_back(cur_bin.volume/(BIN_HEIGHT * BIN_LENGTH * BIN_WIDTH));
        features.push_back(*std::max_element(x.max_pool.begin(), x.max_pool.end()));
        features.push_back(*std::min_element(x.min_pool.begin(), x.min_pool.end()));
        int max_pool_idx = 0, min_pool_idx = 0, avg_pool_idx = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < x.max_pool.size() / 4; j++, max_pool_idx++) {
                features.push_back(x.max_pool[max_pool_idx] * 1.0 / BIN_HEIGHT);
            }
            for (int j = 0; j < x.min_pool.size() / 4; j++, min_pool_idx++) {
                features.push_back(x.min_pool[min_pool_idx] * 1.0 / BIN_HEIGHT);
            }
            for (int j = 0; j < x.avg_pool.size() / 4; j++, avg_pool_idx++) {
                features.push_back(x.avg_pool[avg_pool_idx] * 1.0 / BIN_HEIGHT);
            }
        }
        for (int i : x.border_features) {
            features.push_back(i * 1.0 / BIN_HEIGHT);
        }
        int cur_size = features.size();
        for (int i = cur_size; i < BIAS_HOLE+4*POOL_PARAMS+BOUNDARY_PARAMS; i++) {
            features.push_back(0);
        }
        return features;
    }

    performance_metric execute(Sim &simulator, int lookahead) {
        simulator.size_of_box_stream = boxes.size();
        write_file.open(write_file_name);
        for (auto box : boxes) {
            // std::cout << box.x << " " << box.y << " " << box.z << "\n";
            int flag = 0;
            if (put_box(simulator, box)) {
                // std::cout<<"put"<<"\n";
                flag = 1;
            }
            if (!flag && simulator.open_new_bin()) {
                put_box(simulator, box);
            }
        }
        write_file.close();

        return simulator.get_performance_metric(1);
    }
};