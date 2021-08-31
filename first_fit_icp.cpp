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
    First_Fit_Icp(GenerateBox gb, Sim &simulator) : Base(gb, simulator)
    {
    }
    bool put_box(Sim &simulator, int bin_id, vector_3d box)
    {
        std::vector<std::vector<int>> cur_state = simulator.bin_instances[bin_id].get_state();
        // bin_instance.print_state();
        std::vector<std::pair<vector_3d, vector_3d>> icpbcp_list = simulator.bin_instances[bin_id].get_icbp_list();
        //precompute_max_min(cur_state);
        auto idx_ori = get_action(icpbcp_list, cur_state, box);

        // std::cout << idx_ori.first << "\n";
        if (idx_ori.first >= 0)
        {
            return simulator.step(bin_id, idx_ori.first, box, idx_ori.second);
        }
        else
        {
            //std::cout << "could not place the box" << std::endl;
        }
        return 0;
    }
    int evaluate(std::vector<eval_feature> &features)
    {
        // int total_boxes_put = 0;
        // int n = features.size();
        // for (int i = 0; i < n; i++)
        // {
        //     total_boxes_put += features[i].new_boxes_put_count;
        // }

        // if (total_boxes_put == 0)
        //     return -1;
        // long long max_var = INT_MIN;
        // long long min_var = INT_MAX;
        // long long max_max = INT_MIN;
        // long long min_max = INT_MAX;
        // long long max_min = INT_MIN;
        // long long min_min = INT_MAX;
        // int max_box_area = INT_MIN;
        // int min_box_area = INT_MAX;
        // for (int i = 0; i < n; i++)
        // {
        //     max_var = std::max(max_var, features[i].mul_variance);
        //     min_var = std::min(min_var, features[i].mul_variance);
        //     max_max = std::max(max_max, features[i].mul_maximum);
        //     min_max = std::min(min_max, features[i].mul_maximum);
        //     max_min = std::max(max_min, features[i].mul_minimum);
        //     min_min = std::min(min_min, features[i].mul_minimum);
        //     max_box_area = std::max(max_box_area, features[i].boxArea);
        //     min_box_area = std::min(min_box_area, features[i].boxArea);
        //     // std::cout << max_var << " " << min_var << std::endl;
        //     // std::cout << max_max << " " << min_max << std::endl;
        //     // std::cout << max_min << " " << min_min << std::endl;
        //     // std::cout << max_box_area << " " << min_box_area << std::endl;
        // }
        // for (int i = 0; i < n; i++)
        // {
        //     features[i].score -= features[i].newly_opened_bins_count * 1000000;
        //     features[i].score -= 1.0 * (1 + features[i].mul_variance - min_var) / (1 + max_var - min_var);
        //     features[i].score += 100.0 * (1 + features[i].boxArea - min_box_area) / (1 + max_box_area - min_box_area);
        //     features[i].score -= 100.0 * (1 + features[i].mul_maximum - min_max) / (1 + max_max - min_max);
        //     //features[i].score -= 1.0 * (1 + features[i].mul_minimum - min_min) / (1 + max_min - min_min);
        //     //features[i].score = features[i].box_to_be_placed_idx;
        // }
        // //std::cout << "feature score: " << features[0].score << std::endl;
        // int max_score = INT_MIN;
        // int idx = -1;
        // for (int i = 0; i < n; i++)
        // {
        //     //std::cout << features[i].score << " " << idx << std::endl;
        //     if (max_score < features[i].score)
        //     {
        //         max_score = features[i].score;
        //         idx = features[i].box_to_be_placed_idx;
        //     }
        // }
        // //std::cout << max_score << " " << idx << std::endl;
        // return idx;
        return 0;
    }
    void extract_state_features(Sim &temp_simulator, Sim &simulator, std::vector<int> &changed_bin_instances, eval_feature &feature)
    {
        // feature.score = INT_MAX;
        // //std::cout << simulator.total_number_of_boxes_placed << " " << temp_simulator.total_number_of_boxes_placed << std::endl;
        // feature.new_boxes_put_count = temp_simulator.total_number_of_boxes_placed - simulator.total_number_of_boxes_placed;
        // feature.newly_opened_bins_count = temp_simulator.bin_instances.size() - simulator.bin_instances.size();
        // feature.mul_maximum = 1;
        // feature.mul_minimum = 1;
        // feature.mul_variance = 1;

        // for (int i : changed_bin_instances)
        // {
        //     // if (!b.recentlyChanged)
        //     //     continue;
        //     auto b = temp_simulator.bin_instances[i];
        //     int tmp_min = INT_MAX;
        //     int tmp_max = INT_MIN;
        //     int tmp_var = 0;
        //     auto grid = b.get_state();
        //     for (int i = 1; i < BIN_WIDTH; i++)
        //     {
        //         for (int j = 1; j < BIN_LENGTH; j++)
        //         {
        //             tmp_var += (grid[i][j] != grid[i - 1][j]) + (grid[i][j] != grid[i][j - 1]);
        //             tmp_min = std::min(tmp_min, grid[i][j]);
        //             tmp_max = std::max(tmp_min, grid[i][j]);
        //         }
        //     }
        //     feature.mul_maximum *= (tmp_max + 1);
        //     feature.mul_minimum *= (tmp_min + 1);
        //     feature.mul_variance *= (tmp_var + 1);
        //     feature.variance.push_back(tmp_var);
        //     feature.minimum.push_back(tmp_min);
        //     feature.maximum.push_back(tmp_max);
        // }
        return;
    }

    performance_metric execute(Sim &simulator, int lookahead)
    {
        simulator.size_of_box_stream = boxes.size();
        reverse(boxes.begin(), boxes.end());
        while (boxes.size() > 0)
        {
            //trying diff permutation
            std::vector<int> box_idx;
            for (int i = 0; i < lookahead && i < boxes.size(); i++)
            {
                box_idx.push_back((int)boxes.size() - i - 1);
            }
            std::sort(box_idx.begin(), box_idx.end());
            std::vector<int> changed_bin_instances;
            std::vector<eval_feature> features(factorial(lookahead));
            int feature_id = 0;
            do
            {
                auto temp_simulator = simulator;
                //trying new permuation
                for (int idx : box_idx)
                {
                    auto box = boxes[idx];
                    int flag = 0;
                    for (int i = 0; i < temp_simulator.bin_instances.size(); i++)
                    {
                        if (temp_simulator.bin_instances[i].is_open())
                        {
                            if (put_box(temp_simulator, i, box))
                            {
                                flag = 1;
                                changed_bin_instances.push_back(i);
                                break;
                            }
                        }
                    }
                    if (!flag && temp_simulator.open_new_bin())
                    {
                        put_box(temp_simulator, temp_simulator.bin_instances.size() - 1, box);
                        changed_bin_instances.push_back(temp_simulator.bin_instances.size() - 1);
                    }
                }
                // features[feature_id].box_to_be_placed_idx = box_idx[0];
                // features[feature_id].boxArea = boxes[box_idx[0]].x * boxes[box_idx[0]].y;
                extract_state_features(temp_simulator, simulator, changed_bin_instances, features[feature_id]);
                feature_id++;
            } while (std::next_permutation(box_idx.begin(), box_idx.end()));
            //std::cout << features.size() << std::endl;
            int box_to_be_placed = evaluate(features);
            //std::cout << box_to_be_placed << std::endl;
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
            for (int i = 0; i < simulator.bin_instances.size(); i++)
            {
                if (simulator.bin_instances[i].is_open())
                {
                    if (put_box(simulator, i, box))
                    {
                        flag = 1;
                        break;
                    }
                }
            }
            if (!flag && simulator.open_new_bin())
            {
                put_box(simulator, simulator.bin_instances.size() - 1, box);
            }
            boxes.erase(boxes.begin() + box_to_be_placed);
        }

        return simulator.get_performance_metric();
    }
};