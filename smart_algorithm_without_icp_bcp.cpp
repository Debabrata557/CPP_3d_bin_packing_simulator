
/*

Take features from the complete state.
*/

#include <algorithm>
#include <fstream>
#include "base.h"

class Smart_Algorithm_WithoutICP_BCP: public Base {
   private:
   std::ofstream write_file;
   std::string write_file_name = "details.txt";
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
    vector_3d get_action(Bin &cur_bin, vector_3d &dim) {
        std::vector<std::vector<int>> cur_state = cur_bin.get_state();
        int lx = dim.x, ly = dim.y, lz = dim.z;
        double max_score=-DBL_MAX;
        Bin after_state;
        eval_feature x;
        std::vector<double> features;
        std::pair<int, int> idx = {-1,-1};
        int ori = -1;
        precompute_max_min(cur_state);
        for (int i = 0; i < BIN_WIDTH; i+=10)
        {
            for (int j = 0; j < BIN_LENGTH; j+=10)
            {
                //std::cout<<i<<" "<<j<<std::endl;
                if (check_with_precomputation(cur_state, {i, j}, dim)) {
                    int holes = find_holes(cur_state, {i, j}, dim);
                    after_state = cur_bin;
                    after_state.update_state({i, j}, dim);
                    x.holes = holes;
                    features = extract_state_features_with_symmetry(after_state, x, dim, i, j);
                    // double temp_max = evaluate(features);
                    double temp_max = evaluate_with_symmetry(features);
                    if(temp_max > max_score){
                        idx = {i,j};
                        max_score = temp_max;
                        ori = 0;
                    }
                }
                vector_3d rotated_dim={dim.y, dim.x, dim.z};
                if (check_with_precomputation(cur_state, {i, j}, rotated_dim)) {
                    int holes = find_holes(cur_state, {i, j}, rotated_dim);
                    after_state = cur_bin;
                    after_state.update_state({i, j}, rotated_dim);
                    x.holes = holes;
                    features = extract_state_features_with_symmetry(after_state, x, rotated_dim, i, j);
                    // double temp_max = evaluate(features);
                    double temp_max = evaluate_with_symmetry(features);
                    if(temp_max > max_score){
                        idx = {i,j};
                        max_score = temp_max;
                        ori = 1;
                    }
                }

            }
        }
        return {idx.first, idx.second, ori};
    }

   public:
    Smart_Algorithm_WithoutICP_BCP() {

    }
    Smart_Algorithm_WithoutICP_BCP(GenerateBox gb, Sim &simulator,const std::vector<double>&params) : Base(gb, simulator) {
        this->params = params;
    }
    bool put_box(Sim &simulator, int bin_id, vector_3d box) {
        //std::vector<std::vector<int>> cur_state = simulator.bin_instances[bin_id].get_state();
        // bin_instance.print_state();
        //precompute_max_min(cur_state);
        auto idx_ori = get_action(simulator.bin_instances[bin_id], box);
        // std::cout << idx_ori.first << "\n";
        if (idx_ori.x >= 0) {
            int height=simulator.step(bin_id, {idx_ori.x,idx_ori.y}, box, idx_ori.z);
            write_file<<bin_id<<" "<<box.x<<" "<<box.y<<" "<<box.z<<" "<<idx_ori.x<<" "<<idx_ori.y<<" "<<height<<" "<<idx_ori.z<<"\n"; 
            return height!=-1;
        } else {
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
        int extra_feature=3;
        //assert params.size()==features.size();
        int j=0;
        for (int i = 0; i < params.size()-extra_feature; i++,j++) {
            sum += features[j] * params[i];
        }
        for (int i = 0; i < params.size()-extra_feature; i++,j++) {
            sum += features[j] * params[i];
        }
        for (int i = 0; i < params.size()-extra_feature; i++, j++) {
            sum += features[j] * params[i];
        }
        for (int i = 0; i < params.size()-extra_feature; i++, j++) {
            sum += features[j] * params[i];
        }
        
        for(int i=extra_feature;i>=1;i--,j++){
            sum+=features[j]*params[(int)params.size()-i];
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
        
        
        for (int i = start_x; i < end_x/2; i += stride) {
            for (int j = start_y; j < end_y/2; j += stride) {
                int temp_max = 0;
                int temp_min = 0;
                double mean_height = 0;
                int cell_count = 0;
                for (int inner_i = i; inner_i <= std::min(i + filter_size - 1, end_x/2); inner_i++) {
                    for (int inner_j = j; inner_j <= std::min(j + filter_size - 1, end_y/2); inner_j++) {
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
        for (int i = start_x;i < end_x / 2; i += stride) {
            for (int j = end_y-1; j >= end_y / 2; j -= stride) {
                int temp_max = 0;
                int temp_min = 0;
                double mean_height = 0;
                int cell_count = 0;
                for (int inner_i = i; inner_i <= std::min(i + filter_size - 1, end_x); inner_i++) {
                    for (int inner_j = j; inner_j >= std::max(j - filter_size + 1, end_y/2); inner_j--) {
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
        for (int i = end_x-1; i >= end_x / 2; i -= stride) {
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
            for (int j = end_y-1; j >= end_y / 2; j -= stride) {
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

    void extract_border_feature(std::vector<std::vector<int>> &after_state,vector_3d &dim, int pos_x, int pos_y, eval_feature &x){
        int sum=0;
        for(int i=pos_x;i<pos_x+dim.x;i++){
            if(pos_y>0){
                sum+=abs(after_state[i][pos_y]-after_state[i][pos_y-1]);
            }
            if(pos_y<BIN_LENGTH-1){
                sum+=abs(after_state[i][pos_y+1]-after_state[i][pos_y]);
            }
        }
        for(int i=pos_y;i<pos_y+dim.y;i++){
            if(pos_x>0){
                sum+=abs(after_state[pos_x][i]-after_state[pos_x-1][i]);
            }
            if(pos_x<BIN_WIDTH-1){
                sum+=abs(after_state[pos_x+1][i]-after_state[pos_x][i]);
            }
        }
        x.border_diff_height=sum;
    }
    std::vector<double> extract_state_features(Bin cur_bin, eval_feature &x, vector_3d &dim, int pos_x, int pos_y) {
        auto cur_state = cur_bin.get_state();
        std::vector<double>features;
        int start_x = 0;
        int start_y = 0;
        int end_x = BIN_WIDTH;
        int end_y = BIN_LENGTH;
        // extract_pool_features_with_symmetry(cur_state,start_x,end_x,start_y,end_y,x,STRIDE,FILTER_SIZE);
        extract_pool_features(cur_state, start_x, end_x, start_y, end_y, x, STRIDE, FILTER_SIZE);

        extract_border_feature(cur_state, dim, pos_x, pos_y, x);
        for(int i:x.max_pool){
            features.push_back(i*1.0/BIN_HEIGHT);
        }
        for (int i : x.min_pool) {
            features.push_back(i*1.0/BIN_HEIGHT);
        }
        for (int i : x.avg_pool) {
            features.push_back(i*1.0/BIN_HEIGHT);
        }
        features.push_back(x.holes/(0.5*BIN_HEIGHT*BIN_LENGTH*BIN_WIDTH));
        double max_border_feature_val=2.0*(MAX_BOX_LENGTH+MAX_BOX_WIDTH)*BIN_HEIGHT;
        features.push_back(x.border_diff_height/max_border_feature_val);
        features.push_back(1);
        return features;
    }

    std::vector<double> extract_state_features_with_symmetry(Bin cur_bin, eval_feature &x, vector_3d &dim, int pos_x, int pos_y) {
        auto cur_state = cur_bin.get_state();
        std::vector<double> features;
        int start_x = 0;
        int start_y = 0;
        int end_x = BIN_WIDTH;
        int end_y = BIN_LENGTH;
        extract_pool_features_with_symmetry(cur_state,start_x,end_x,start_y,end_y,x,STRIDE,FILTER_SIZE);
        // extract_pool_features(cur_state, start_x, end_x, start_y, end_y, x, STRIDE, FILTER_SIZE);

        extract_border_feature(cur_state, dim, pos_x, pos_y, x);
        int max_pool_idx=0,min_pool_idx=0,avg_pool_idx=0;
        for(int i=0;i<4;i++){
            for(int j=0;j<x.max_pool.size()/4;j++,max_pool_idx++){
                features.push_back(x.max_pool[max_pool_idx] * 1.0 / BIN_HEIGHT);
            }
            for (int j = 0; j < x.min_pool.size() / 4; j++,min_pool_idx++) {
                features.push_back(x.min_pool[min_pool_idx] * 1.0 / BIN_HEIGHT);
            }

            for (int j = 0; j < x.avg_pool.size() / 4; j++,avg_pool_idx++) {
                features.push_back(x.avg_pool[avg_pool_idx] * 1.0 / BIN_HEIGHT);
            }
            
        }

        features.push_back(x.holes / (0.5 * BIN_HEIGHT * BIN_LENGTH * BIN_WIDTH));
        double max_border_feature_val = 2.0 * (MAX_BOX_LENGTH + MAX_BOX_WIDTH) * BIN_HEIGHT;
        features.push_back(x.border_diff_height / max_border_feature_val);
        features.push_back(1);
        return features;
    }

    performance_metric execute(Sim &simulator, int lookahead) {
        simulator.size_of_box_stream=boxes.size();
        write_file.open(write_file_name);
        for (auto box : boxes) {
            //std::cout << box.x << " " << box.y << " " << box.z << "\n";
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
        write_file.close();

        return simulator.get_performance_metric(1);
    }
};