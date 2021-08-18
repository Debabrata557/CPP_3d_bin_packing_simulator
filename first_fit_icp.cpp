#include <algorithm>

#include "base.h"

class First_Fit_Icp : public Base
{
private:
    std::pair<int, int> get_action(std::vector<std::pair<vector_3d, vector_3d>> &icpbcp_list, std::vector<std::vector<int>> &state, vector_3d &dim)
    {
        int lx = dim.x, ly = dim.y, lz = dim.z;
        for (int i = 0; i < icpbcp_list.size(); i++)
        {
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, dim))
            {
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;

                if ((x_diff >= lx && y_diff >= ly && z_diff >= lz))
                {
                    return {i, 0};
                }
            }
            if (check_without_precomputation(state, {icpbcp_list[i].first.x, icpbcp_list[i].first.y}, {dim.y, dim.x, dim.z}))
            {
                auto icp_bcp = icpbcp_list[i];
                int x_diff = icp_bcp.second.x - icp_bcp.first.x;
                int y_diff = icp_bcp.second.y - icp_bcp.first.y;
                int z_diff = icp_bcp.second.z - icp_bcp.first.z;

                if ((x_diff >= ly && y_diff >= lx && z_diff >= lz))
                {
                    return {i, 1};
                }
            }
        }
        return {-1, 0};
    }

public:
    First_Fit_Icp()
    {
    }
    First_Fit_Icp(GenerateBox gb, std::vector<Bin> &bin_instances) : Base(gb, bin_instances)
    {
    }
    bool put_box(Bin &bin_instance, vector_3d box, double &total_volume)
    {
        std::vector<std::vector<int>> cur_state = bin_instance.get_state();
        // bin_instance.print_state();
        std::vector<std::pair<vector_3d, vector_3d>> icpbcp_list = bin_instance.get_icbp_list();
        //precompute_max_min(cur_state);
        auto idx_ori = get_action(icpbcp_list, cur_state, box);
        // std::cout << idx_ori.first << "\n";
        if (idx_ori.first >= 0)
        {
            if (idx_ori.second == 0)
            {
                if (bin_instance.update_icpbcp_list(idx_ori.first, box) && bin_instance.update_state({icpbcp_list[idx_ori.first].first.x, icpbcp_list[idx_ori.first].first.y}, box))
                {
                    total_volume += box.x * box.y * box.z;
                    bin_instance.volume += box.x * box.y * box.z;
                    bin_instance.no_of_boxes_placed++;
                    return 1;
                }
                else
                {
                    std::cout << "exception occured" << std::endl;
                    return 0;
                }
            }
            else
            {
                if (bin_instance.update_icpbcp_list(idx_ori.first, {box.y, box.x, box.z}) && bin_instance.update_state({icpbcp_list[idx_ori.first].first.x, icpbcp_list[idx_ori.first].first.y}, {box.y, box.x, box.z}))
                {
                    total_volume += box.x * box.y * box.z;
                    bin_instance.volume += box.x * box.y * box.z;
                    bin_instance.no_of_boxes_placed++;
                    return 1;
                }
                else
                {
                    std::cout << "exception occured" << std::endl;
                    return 0;
                }
            }
            //std::cout << "icpbcp_size " << icpbcp_list.size() << "\n";
        }
        else
        {
            //std::cout << "could not place the box" << std::endl;
        }
        return 0;
    }
    int close_bin(std::vector<Bin> &bins)
    {
        int maximum = INT_MIN;
        int idx = -1;
        int n = bins.size();
        for (int i = 0; i < n; i++)
        {
            if (bins[i].is_open() && bins[i].volume > maximum)
            {
                idx = i;
                maximum = bins[i].volume;
            }
        }
        bins[idx].open = false;
        return idx;
    }
    int evaluate(std::vector<eval_feature> &features)
    {
        int total_boxes_put = 0;
        int n = features.size();
        for (int i = 0; i < n; i++)
        {
            total_boxes_put += features[i].new_boxes_put_count;
        }
        if (total_boxes_put == 0)
            return -1;
        long long max_var = INT_MIN;
        long long min_var = INT_MAX;
        long long max_max = INT_MIN;
        long long min_max = INT_MAX;
        long long max_min = INT_MIN;
        long long min_min = INT_MAX;
        int max_box_area = INT_MIN;
        int min_box_area = INT_MAX;
        for (int i = 0; i < n; i++)
        {
            max_var = std::max(max_var, features[i].mul_variance);
            min_var = std::min(min_var, features[i].mul_variance);
            max_max = std::max(max_max, features[i].mul_maximum);
            min_max = std::min(min_max, features[i].mul_maximum);
            max_min = std::max(max_min, features[i].mul_minimum);
            min_min = std::min(min_min, features[i].mul_minimum);
            max_box_area = std::max(max_box_area, features[i].boxArea);
            min_box_area = std::min(min_box_area, features[i].boxArea);
            // std::cout << max_var << " " << min_var << std::endl;
            // std::cout << max_max << " " << min_max << std::endl;
            // std::cout << max_min << " " << min_min << std::endl;
            // std::cout << max_box_area << " " << min_box_area << std::endl;
        }
        for (int i = 0; i < n; i++)
        {
            features[i].score -= features[i].newly_opened_bins_count * 1000000;
            features[i].score -= 10000.0 * (1 + features[i].mul_variance - min_var) / (1 + max_var - min_var);
            features[i].score += 100.0 * (1 + features[i].boxArea - min_box_area) / (1 + max_box_area - min_box_area);
            features[i].score -= 1.0 * (1 + features[i].mul_maximum - min_max) / (1 + max_max - min_max);
            features[i].score -= 1.0 * (1 + features[i].mul_minimum - min_min) / (1 + max_min - min_min);
            //features[i].score = features[i].box_to_be_placed_idx;
        }

        int max_score = INT_MIN;
        int idx = -1;
        for (int i = 0; i < n; i++)
        {
            //std::cout << features[i].score << " " << idx << std::endl;
            if (max_score < features[i].score)
            {
                max_score = features[i].score;
                idx = features[i].box_to_be_placed_idx;
            }
        }
        //std::cout << max_score << " " << idx << std::endl;
        return idx;
    }
    int extract_state_features(std::vector<Bin> &bin_instances, int new_bin_opened, int new_boxes_put, eval_feature &feature)
    {
        feature.score = INT_MAX;
        feature.new_boxes_put_count = new_boxes_put;
        feature.newly_opened_bins_count = new_bin_opened;
        feature.mul_maximum = 1;
        feature.mul_minimum = 1;
        feature.mul_variance = 1;

        for (Bin b : bin_instances)
        {
            if (!b.recentlyChanged)
                continue;
            int tmp_min = INT_MAX;
            int tmp_max = INT_MIN;
            int tmp_var = 0;
            auto grid = b.get_state();
            for (int i = 1; i < BIN_WIDTH; i++)
            {
                for (int j = 1; j < BIN_LENGTH; j++)
                {
                    tmp_var += (grid[i][j] != grid[i - 1][j]) + (grid[i][j] != grid[i][j - 1]);
                    tmp_min = std::min(tmp_min, grid[i][j]);
                    tmp_max = std::max(tmp_min, grid[i][j]);
                }
            }
            feature.mul_maximum *= (tmp_max + 1);
            feature.mul_minimum *= (tmp_min + 1);
            feature.mul_variance *= (tmp_var + 1);
            feature.variance.push_back(tmp_var);
            feature.minimum.push_back(tmp_min);
            feature.maximum.push_back(tmp_max);
        }
    }

    performance_metric execute(int max_bin_limit, int max_open_bins, int lookahead)
    {
        double total_volume = 0;
        double no_of_bins_used = 0;
        double no_of_boxes_put = 0;
        performance_metric pm;
        //int to_be_closed = 0;
        int cur_open = 0;
        pm.total_number_of_boxes = boxes.size();
        reverse(boxes.begin(), boxes.end());
        while (boxes.size() > 0)
        {
            //std::cout << boxes.size() << std::endl;
            //trying diff permutation
            std::vector<int> box_idx;
            for (int i = 0; i < lookahead && i < boxes.size(); i++)
            {
                box_idx.push_back((int)boxes.size() - i - 1);
            }
            std::sort(box_idx.begin(), box_idx.end());
            std::vector<eval_feature> features(factorial(lookahead));
            int feature_id = 0;
            do
            {
                auto temp_bin_instances = bin_instances;
                int temp_no_of_bins_used = no_of_bins_used, temp_cur_open = cur_open, temp_no_of_boxes_put = no_of_boxes_put;
                double temp = 0;
                for (int i = 0; i < temp_bin_instances.size(); i++)
                    temp_bin_instances[i].recentlyChanged = false;
                //trying new permuation
                for (int idx : box_idx)
                {
                    auto box = boxes[idx];
                    int flag = 0;
                    for (int i = 0; i < temp_bin_instances.size(); i++)
                    {
                        if (temp_bin_instances[i].is_open())
                        {
                            if (put_box(temp_bin_instances[i], box, temp))
                            {
                                flag = 1;
                                temp_no_of_boxes_put++;
                                temp_bin_instances[i].recentlyChanged = true;
                                break;
                            }
                        }
                    }
                    if (!flag && temp_no_of_bins_used < max_bin_limit)
                    {
                        Bin new_bin = Bin();
                        temp_bin_instances.push_back(new_bin);
                        temp_no_of_bins_used++;
                        temp_cur_open++;
                        if (temp_cur_open > max_open_bins)
                        {
                            close_bin(temp_bin_instances);
                            // temp_bin_instances[temp_to_be_closed].open = false;
                            // temp_to_be_closed++;
                            temp_cur_open--;
                        }
                        put_box(temp_bin_instances.back(), box, temp);
                        temp_bin_instances.back().recentlyChanged = true;
                        temp_no_of_boxes_put++;
                    }
                }
                features[feature_id].box_to_be_placed_idx = box_idx[0];
                features[feature_id].boxArea = boxes[box_idx[0]].x * boxes[box_idx[0]].y;
                extract_state_features(temp_bin_instances, temp_no_of_bins_used - no_of_bins_used, temp_no_of_boxes_put - no_of_boxes_put, features[feature_id]);
                feature_id++;
            } while (std::next_permutation(box_idx.begin(), box_idx.end()));

            int box_to_be_placed = evaluate(features);
            if (box_to_be_placed < 0)
            {
                //remove all
                for (int idx : box_idx)
                {
                    boxes.erase(boxes.begin() + idx);
                }
                continue;
            }
            //end of diff permuation
            //start actually putting the box
            //std::cout << box_to_be_placed << "vol: " << boxes[box_to_be_placed].x * boxes[box_to_be_placed].y * boxes[box_to_be_placed].z << std::endl;
            auto box = boxes[box_to_be_placed];
            int flag = 0;
            for (int i = 0; i < bin_instances.size(); i++)
            {
                if (bin_instances[i].is_open())
                {
                    if (put_box(bin_instances[i], box, total_volume))
                    {
                        flag = 1;
                        no_of_boxes_put++;
                        break;
                    }
                }
            }
            if (!flag && no_of_bins_used < max_bin_limit)
            {
                Bin new_bin = Bin();
                bin_instances.push_back(new_bin);
                no_of_bins_used++;
                cur_open++;
                if (cur_open > max_open_bins)
                {
                    int idx = close_bin(bin_instances);
                    std::cout << "efficiency  closed: " << bin_instances[idx].volume / (120.0 * 180 * 120) << std::endl;
                    //bin_instances[to_be_closed].open = false;
                    //to_be_closed++;
                    cur_open--;
                }
                put_box(bin_instances.back(), box, total_volume);
                no_of_boxes_put++;
            }
            boxes.erase(boxes.begin() + box_to_be_placed);
        }

        // for (int i = 1; i <= bin_instances.size(); i++) {
        //     std::cout << i << " " << bin_instances[i - 1].no_of_boxes_placed << " " << bin_instances[i - 1].volume / (BIN_WIDTH * BIN_HEIGHT * BIN_LENGTH) << "\n";
        // }
        // std::cout << no_of_bins_used << "\n";
        total_volume = 0;
        no_of_bins_used = 0;
        no_of_boxes_put = 0;
        for (int i = 0; i < bin_instances.size(); i++)
        {
            if (!bin_instances[i].is_open())
            {
                total_volume += bin_instances[i].volume;
                no_of_bins_used++;
                no_of_boxes_put += bin_instances[i].no_of_boxes_placed;
            }
        }
        double efficiency = total_volume / (double)(no_of_bins_used * BIN_WIDTH * BIN_HEIGHT * BIN_LENGTH);
        pm.efficiency = efficiency;
        pm.number_of_bins_used = no_of_bins_used;
        pm.number_of_boxes_successfully_put = no_of_boxes_put;

        return pm;
    }
};