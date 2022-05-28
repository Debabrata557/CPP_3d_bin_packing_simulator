
/*

Take features from the complete state.
*/

#include <algorithm>
#include <fstream>
#include "base.h"

class Smart_Algorithm4 : public Base {
   private:
    std::ofstream write_file;
    std::string write_file_name = "details.txt";
    std::vector<double> params;
    int score(const std::vector<std::vector<int>> &state, const std::pair<int, int> &pos, const vector_3d &dim)
    {
        int lx = dim.x;
        int ly = dim.y;

        int minx = pos.first;
        int maxx = pos.first + lx - 1;
        int miny = pos.second;
        int maxy = pos.second + ly - 1;
        int max_height = compute_max_min(minx, maxx, miny, maxy).first;
        return max_height;
    }
    std::vector<double> walle_g_params(std::vector<std::vector<int>> &cur_state, std::vector<int> pos, const vector_3d &dim){
        double lx=dim.x, ly=dim.y, lz=dim.z;
        int minx = pos[0], maxx = pos[0] + lx - 1, miny = pos[1], maxy = pos[1] + ly - 1;
        std::vector<double> gvar(4, 0);
        std::vector<double> ghigh{ly, lx, ly, lx};
        std::vector<double> gflush(4, 0);
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
        return {std::accumulate(gvar.begin(), gvar.end(), 0.0), std::accumulate(ghigh.begin(), ghigh.end(), 0.0),std::accumulate(gflush.begin(), gflush.end(), 0.0)};
    }
    double calculate_walle_score(std::vector<std::vector<int>> &cur_state, std::vector<int> pos, const vector_3d &dim) {
        std::vector<double> params=walle_g_params(cur_state, pos, dim);
        double alpha1=0.75,alpha2=1,alpha3=1,alpha4=0.01,alpha5=1;
        return alpha1*params[0]+alpha3*params[2]+alpha2*params[1]-alpha4*pos[0]*pos[1]-alpha5*(dim.z+cur_state[pos[0]][pos[1]]);
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
    void get_heuristic_based_icbp_list(std::vector<vector_3d> &heuristic_based_icpbcp_list, std::vector<std::vector<int>> &state, vector_3d dim) {
        precompute_max_min(state);
        int lx = dim.x, ly = dim.y, lz = dim.z;
        int first_fit_flag1=0, first_fit_flag2=0;
        vector_3d pos_orientation1 = {-40, -40, -1}, pos_orientation2 = {-40, -40, -1};
        int min_height1 = INT_MAX, min_height2 = INT_MAX;
        double walleScoreMax1=DBL_MIN, walleScoreMax2=DBL_MIN;
        std::vector<vector_3d> walle_pos_ori1, walle_pos_ori2;
        for (int i = 0; i < BIN_WIDTH; i++)
        {
            for (int j = 0; j < BIN_LENGTH; j++)
            {
                int flag = check_with_precomputation(state, {i, j, 0}, {lx, ly, lz});
                if(flag){
                    heuristic_based_icpbcp_list.push_back({i, j, 0});
                }
                flag = check_with_precomputation(state, {i, j, 0}, {ly, lx, lz});
                if(flag){
                    heuristic_based_icpbcp_list.push_back({i, j, 1});
                }
            }
        }
        // if(pos_orientation1.z!=-1){
        //     heuristic_based_icpbcp_list.push_back({pos_orientation1.x, pos_orientation1.y, 0});
        // }
        // if(pos_orientation2.z!=-1){
        //     heuristic_based_icpbcp_list.push_back({pos_orientation2.x, pos_orientation2.y, 1});
        // }
        // if(walle_pos_ori1.size()!=0){
        //     heuristic_based_icpbcp_list.insert(heuristic_based_icpbcp_list.end(),walle_pos_ori1.begin(),walle_pos_ori1.end());
        // }
        // if(walle_pos_ori2.size()!=0){
        //     heuristic_based_icpbcp_list.insert(heuristic_based_icpbcp_list.end(),walle_pos_ori2.begin(),walle_pos_ori2.end());
        // }
        return;
    }
    int find_holes(std::vector<std::vector<int>> cur_state, std::pair<int, int> xy, vector_3d dim) {
        dim={dim.x+4, dim.y+4, dim.z};
        int max_height = grid_max(cur_state, xy.first, xy.first + dim.x, xy.second, xy.second + dim.y);
        int holes = 0;
        for (int i = xy.first; i < xy.first + dim.x; i++) {
            for (int j = xy.second; j < xy.second + dim.y; j++) {
                holes += max_height - cur_state[i][j];
            }
        }
        return holes;
    }
    double find_side_holes(const std::vector<std::vector<int>> &after_state, std::vector<int> pos, vector_3d dim){
        dim={dim.x+4, dim.y+4, dim.z};
        double holes=0;
        int start_x= pos[0],start_y_for_left=pos[1]-1,start_x_for_below = pos[0]+dim.x,start_y = pos[1],start_y_for_right=pos[1]+dim.y,start_x_for_above=pos[0]-1;
        
        int left_hole_length = INT_MAX,right_hole_length=INT_MAX,upper_hole_length = INT_MAX,lower_hole_length = INT_MAX;
        int cur_h =-1;
        if(start_y_for_left>=0)
            cur_h= after_state[start_x][start_y_for_left];
        for(int i=start_x;i<start_x+dim.x && i<BIN_WIDTH;i++){
            int l=0;
            while (start_y_for_left-l >=0 && after_state[i][start_y_for_left-l]==cur_h && l<31)
            {
                l++;
            }
            left_hole_length=std::min(l,left_hole_length);
            
        }
        if(start_y_for_right<BIN_LENGTH)
            cur_h = after_state[start_x][start_y_for_right];
        for(int i=start_x;i<start_x+dim.x && i<BIN_WIDTH;i++){
            int l=0;
            while (start_y_for_right+l <BIN_LENGTH && after_state[i][start_y_for_right+l]==cur_h && l<31)
            {
                l++;
            }
            right_hole_length=std::min(l,right_hole_length);
        }
        if(start_x_for_below<BIN_WIDTH)
            cur_h = after_state[start_x_for_below][start_y];
        for(int j=start_y;j<start_y+dim.y && j<BIN_LENGTH ;j++){
            int l=0;
            while (start_x_for_below+l <BIN_WIDTH && after_state[start_x_for_below+l][j]==cur_h && l<31)
            {
                l++;
            }
            lower_hole_length=std::min(l,lower_hole_length);
            
        }
        if(start_x_for_above>=0)
            cur_h = after_state[start_x_for_above][start_y];
        for(int j=start_y;j<start_y+dim.y && j<BIN_LENGTH ;j++){
            int l=0;
            while (start_x_for_above-l >=0 && after_state[start_x_for_above-l][j]==cur_h && l<31)
            {
                l++;
            }
            upper_hole_length=std::min(l,upper_hole_length);
            
        }
        if(left_hole_length>=30)left_hole_length=0;
        if(right_hole_length>=30)right_hole_length=0;
        if(upper_hole_length>=30)upper_hole_length=0;
        if(lower_hole_length>=30)lower_hole_length=0;
        return dim.x*(left_hole_length+right_hole_length)+dim.y*(upper_hole_length+lower_hole_length);




    }
    std::vector<int> get_action(Sim &simulator, vector_3d &dim) {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        double max_score = -DBL_MAX;
        int idx_i = -1, idx_j=-1;
        int ori = -1;
        int bin_id = -1;
        std::vector<std::pair<double, int>> efficiency_sorted_bins;
        for (int s = 0; s < simulator.bin_instances.size(); s++) {
            auto cur_bin = simulator.bin_instances[s];
            if (!cur_bin.is_open()) continue;
            efficiency_sorted_bins.push_back({cur_bin.volume, s});

        }

        for (auto b: efficiency_sorted_bins) {
            int s=b.second;
            auto cur_bin = simulator.bin_instances[s];
            if (!cur_bin.is_open()) continue;
            std::vector<vector_3d> heuristic_based_icpbcp_list;
            auto cur_state = cur_bin.get_state();
            get_heuristic_based_icbp_list(heuristic_based_icpbcp_list, cur_state, dim);
            for(auto pos_ori: heuristic_based_icpbcp_list){
                if(pos_ori.z==0){
                    std::pair<int, int> start_point = {pos_ori.x, pos_ori.y};
                    int holes = find_holes(cur_state, start_point, dim);
                    std::vector<double> walle_features=walle_g_params(cur_state, {start_point.first, start_point.second}, dim);
                    Bin temp_bin = cur_bin;
                    temp_bin.update_state(start_point, dim);
                    temp_bin.volume += dim.x * dim.y * dim.z;
                    eval_feature x;
                    x.holes = holes;
                    x.g_var=walle_features[0];
                    x.g_high=walle_features[1];
                    x.g_flush=walle_features[2];
                    std::vector<double> features = extract_state_features(temp_bin, x, dim, start_point.first, start_point.second);
                    double temp_max = evaluate(features);
                    // std::cout<<"temp_max"<<temp_max<<"\n";
                    if (temp_max > max_score) {
                        idx_i = pos_ori.x;
                        idx_j = pos_ori.y;
                        max_score = temp_max;
                        ori = 0;
                        bin_id = s;
                    }
                }
                else{
                    vector_3d rotated_dim = {dim.y, dim.x, dim.z};
                    std::pair<int, int> start_point = {pos_ori.x, pos_ori.y};
                    int holes = find_holes(cur_state, start_point, rotated_dim);
                    std::vector<double> walle_features=walle_g_params(cur_state, {start_point.first, start_point.second}, rotated_dim);
                    Bin temp_bin = cur_bin;
                    temp_bin.update_state(start_point, rotated_dim);
                    temp_bin.volume += rotated_dim.x * rotated_dim.y * rotated_dim.z;
                    eval_feature x;
                    x.holes = holes;
                    x.g_var=walle_features[0];
                    x.g_high=walle_features[1];
                    x.g_flush=walle_features[2];
                    std::vector<double> features = extract_state_features(temp_bin, x, rotated_dim, start_point.first, start_point.second);
                    double temp_max = evaluate(features);
                    if (temp_max > max_score) {
                        idx_i = pos_ori.x;
                        idx_j = pos_ori.y;
                        max_score = temp_max;
                        ori = 1;
                        bin_id = s;
                    }
                }
            }
            if(idx_i!=-1)
                return {idx_i, idx_j, ori, bin_id};

        }

        return {idx_i, idx_j, ori, bin_id};
    }

   public:
    Smart_Algorithm4() {
    }
    Smart_Algorithm4(GenerateBox gb, Sim &simulator, const std::vector<double> &params) : Base(gb, simulator) {
        this->params = params;
    }
    bool put_box(Sim &simulator, vector_3d box) {
        auto pos_ori_bin = get_action(simulator, box);
        if (pos_ori_bin[3] == -1) return 0;
        if(pos_ori_bin[0]>=0){
            int height = simulator.step(pos_ori_bin[3], {pos_ori_bin[0], pos_ori_bin[1]}, box, pos_ori_bin[2]);
            write_file << pos_ori_bin[3] << " " << box.x << " " << box.y << " " << box.z << " " << pos_ori_bin[0] << " " << pos_ori_bin[1] << " " << height << " " << pos_ori_bin[2] << "\n";
            return height != -1;
        }else {
            write_file << pos_ori_bin[3] << " " << box.x << " " << box.y << " " << box.z << " " << pos_ori_bin[0] << " " << pos_ori_bin[1] << " " << -1 << " " << pos_ori_bin[2] << "\n";
        }
        return 0;
    }
    double evaluate(std::vector<double> &features) {
        double sum = 0;
        // assert(params.size()==features.size());
        for (int i = 0; i < features.size()-4*POOL_PARAMS; i++) {
            sum += features[i] * params[i];
        }



        int j = features.size()-4*POOL_PARAMS;

        for (int i = features.size()-POOL_PARAMS; i < features.size(); i++, j++) {
            sum += features[j] * params[i];
            assert(j < features.size());
        }
        for (int i = features.size()-POOL_PARAMS; i < features.size(); i++, j++) {
            sum += features[j] * params[i];
            assert(j < features.size());
        }
        for (int i = features.size()-POOL_PARAMS; i < features.size(); i++, j++) {
            sum += features[j] * params[i];
            assert(j < features.size());
        }
        for (int i = features.size()-POOL_PARAMS; i < features.size(); i++, j++) {
            sum += features[j] * params[i];
            assert(j < features.size());
        }
        return sum;
    }


    

    void extract_border_feature(std::vector<std::vector<int>> &after_state, vector_3d &dim, int pos_x, int pos_y, eval_feature &x) {
        int sum = 0;
        // std::cout<<"\n\n\nstate:";
        // print_state(after_state);
        int margin=2;
        for (int i = pos_x; i < pos_x + dim.x; i += BOUNDARY_STRIDE) {
            if (pos_y-1-margin >= 0) {
                x.border_features.push_back(abs(after_state[i][pos_y+1] - after_state[i][pos_y - 1-margin]));
            } else {
                x.border_features.push_back(0);
            }
            if (pos_y + dim.y+margin < BIN_LENGTH) {
                x.border_features.push_back(abs(after_state[i][pos_y + dim.y+margin] - after_state[i][pos_y + dim.y - 2]));
            } else {
                x.border_features.push_back(0);
            }
        }
        for (int i = pos_y; i < pos_y + dim.y; i += BOUNDARY_STRIDE) {
            if (pos_x-1-margin >=0) {
                x.border_features.push_back(abs(after_state[pos_x+1][i] - after_state[pos_x - 1-margin][i]));
            } else {
                x.border_features.push_back(0);
            }
            if (pos_x + dim.x+margin < BIN_WIDTH) {
                x.border_features.push_back(abs(after_state[pos_x + dim.x+margin][i] - after_state[pos_x + dim.x - 2][i]));
            } else {
                x.border_features.push_back(0);
            }
        }
    }

    std::vector<double> extract_state_features(Bin after_bin, eval_feature &x, vector_3d &dim, int pos_x, int pos_y) {
        auto cur_state = after_bin.get_state();
        std::vector<double> features;
        int start_x = 0;
        int start_y = 0;
        int end_x = BIN_WIDTH;
        int end_y = BIN_LENGTH;
        if(POOL_PARAMS!=0)
            extract_pool_features_with_symmetry(cur_state, start_x, end_x, start_y, end_y, x, STRIDE, FILTER_SIZE);
        extract_border_feature(cur_state, dim, pos_x, pos_y, x);
        std::pair<int, int> max_min=grid_max_min(after_bin.get_state(), 0, BIN_WIDTH, 0, BIN_LENGTH);
        double side_hole=find_side_holes(after_bin.get_state(), {pos_x, pos_y}, dim);
        features.push_back(1);
        features.push_back(x.holes / (BIN_HEIGHT * BIN_LENGTH * BIN_WIDTH));
        features.push_back(side_hole/(BIN_LENGTH * BIN_WIDTH));
        features.push_back(after_bin.no_of_boxes_placed/96.0);
        features.push_back(after_bin.volume / (BIN_HEIGHT * BIN_LENGTH * BIN_WIDTH));
        features.push_back(1.0*max_min.first/BIN_HEIGHT);
        features.push_back(1.0*max_min.second/BIN_HEIGHT);
        //side holes
        // box size large medium small

        features.push_back(x.g_flush/(2*120+4));
        features.push_back(x.g_var/(244*120));
        features.push_back(x.g_high/(2*120+4));

        for (int i : x.border_features) {
            features.push_back(i * 1.0 / BIN_HEIGHT);
        }
        int cur_size = features.size();
        for (int i = cur_size; i < BIAS_HOLE + BOUNDARY_PARAMS; i++) {
            features.push_back(0);
        }
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
        // for(auto p: features){
        //     std::cout<<p<<"  ";
        // }
        // std::cout<<"\n";
        return features;
    }

    performance_metric execute(Sim &simulator, int lookahead) {
        simulator.size_of_box_stream = boxes.size();
        write_file.open(write_file_name);
        // for(auto p: params){
        //     std::cout<<p<<"  ";
        // }
        // std::cout<<"\n";
        for (auto box : boxes) {
            int flag = 0;
            if (put_box(simulator, box)) {
                flag = 1;
            }
            if (!flag && simulator.open_new_bin()) {
                put_box(simulator, box);
            }
        }
        write_file.close();

        return simulator.get_performance_metric(0);
    }
};