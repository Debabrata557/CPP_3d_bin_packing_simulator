/*

Take features from the area around the box where it is placed.

*/

#include <algorithm>
#include <fstream>
#include "base.h"

class Smart_Algorithm2 : public Base {
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
    std::pair<int, int> get_action(Bin &cur_bin, vector_3d &dim) {
        auto icpbcp_list = cur_bin.get_icbp_list();
        auto cur_state = cur_bin.get_state();
        int lx = dim.x, ly = dim.y, lz = dim.z;
        double max_score=-DBL_MAX;
        int idx = -1;
        int ori = -1;
        for (int i = 0; i < icpbcp_list.size(); i++) {
            if (check_without_precomputation(cur_state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim)) {
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;

                if ((x_diff >= lx && y_diff >= ly && z_diff >= lz)) {
                    int holes = find_holes(cur_state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim);
                    Bin temp_bin = cur_bin;
                    temp_bin.update_state({icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim);
                    eval_feature x;
                    x.holes = holes;
                    std::vector<double> features = extract_state_features(temp_bin, x, dim, icpbcp_list[i].first.x, icpbcp_list[i].first.y);
                    double temp_max = evaluate(features);
                    if (temp_max > max_score) {
                        idx = i;
                        max_score = temp_max;
                        ori = 0;
                    }
                }
            }
            vector_3d rotated_dim={dim.y, dim.x, dim.z};
            if (check_without_precomputation(cur_state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, rotated_dim)) {
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;
                if ((x_diff >= lx && y_diff >= ly && z_diff >= lz)) {
                    int holes = find_holes(cur_state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, rotated_dim);
                    Bin temp_bin = cur_bin;
                    temp_bin.update_state({icpbcp_list[i].first.x, icpbcp_list[i].first.y}, rotated_dim);
                    eval_feature x;
                    x.holes = holes;
                    std::vector<double> features = extract_state_features(temp_bin, x, rotated_dim, icpbcp_list[i].first.x, icpbcp_list[i].first.y);
                    double temp_max = evaluate(features);
                    if (temp_max > max_score) {
                        idx = i;
                        max_score = temp_max;
                        ori = 1;
                    }
                }
            }
        }
        return {idx, ori};
    }

   public:
    Smart_Algorithm2() {
    }
    Smart_Algorithm2(GenerateBox gb, Sim &simulator, const std::vector<double> &params) : Base(gb, simulator) {
        this->params = params;
    }
    bool put_box(Sim &simulator, int bin_id, vector_3d box) {
        std::vector<std::vector<int>> cur_state = simulator.bin_instances[bin_id].get_state();
        // bin_instance.print_state();
        std::vector<std::pair<vector_3d, vector_3d>> icpbcp_list = simulator.bin_instances[bin_id].get_icbp_list();
        //precompute_max_min(cur_state);
        auto idx_ori = get_action(simulator.bin_instances[bin_id], box);
        auto icp_bcp=simulator.bin_instances[bin_id].get_icbp_list()[idx_ori.first].first;
        // std::cout << idx_ori.first << "\n";
        if (idx_ori.first >= 0) {
            int height=simulator.step(bin_id, idx_ori.first, box, idx_ori.second);
            write_file<<box.x<<" "<<box.y<<" "<<box.z<<" "<<icp_bcp.x<<" "<<icp_bcp.y<<" "<<height<<" "<<idx_ori.second<<"\n"; 
            return height!=-1;
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

    void extract_pool_features(std::vector<std::vector<int>> &after_state,int start_x,int end_x,int start_y,int end_y, eval_feature &x, int stride, int filter_size) {
        for(int i=start_x;i<end_x;i+=stride){
            for(int j=start_y;j<end_y;j+=stride){
                int temp_max = 0;
                int temp_min = 0;
                double mean_height = 0;
                int cell_count = 0;
                for(int inner_i = i;inner_i<=std::min(i+filter_size-1,end_x);inner_i++){
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
        int start_x=std::max(0,pos_x+dim.x/2-MAX_BOX_WIDTH/2-10);
        int start_y=std::max(0, pos_y+dim.y/2-MAX_BOX_LENGTH/2-10);
        int end_x = std::min(BIN_WIDTH, pos_x + dim.x / 2 + MAX_BOX_WIDTH / 2+11);
        int end_y = std::min(BIN_LENGTH, pos_y + dim.y / 2 + MAX_BOX_LENGTH / 2+11);

        if(start_x==0)
            end_x=EXTRACT_FEATURE_AREA;
        if(start_y==0)
            end_y=EXTRACT_FEATURE_AREA;
        if(end_y==BIN_LENGTH)
            start_y=BIN_LENGTH-EXTRACT_FEATURE_AREA;
        if(end_x==BIN_WIDTH)
            start_x=BIN_WIDTH-EXTRACT_FEATURE_AREA;

        auto cur_state = cur_bin.get_state();
        std::vector<double> features;
        extract_pool_features(cur_state, start_x,start_y,end_x, end_y,x, STRIDE, FILTER_SIZE);
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
        features.push_back(1); //bias
        return features;
    }

    performance_metric execute(Sim &simulator, int lookahead) {
        simulator.size_of_box_stream = boxes.size();
        write_file.open(write_file_name);
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
         write_file.close();
        return simulator.get_performance_metric(1);
    }
};