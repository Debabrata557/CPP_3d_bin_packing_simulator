#include "sim.h"

Sim::Sim()
{
    max_bin_limit = 200;
    max_open_limit = 1;
}
Sim::~Sim()
{
}
int Sim::close_bin()
{
    int maximum = INT_MIN;
    int idx = -1;
    int n = bin_instances.size();
    for (int i = 0; i < n; i++)
    {
        if (bin_instances[i].is_open() && bin_instances[i].volume > maximum)
        {
            idx = i;
            maximum = bin_instances[i].volume;
        }
    }
    bin_instances[idx].open = false;
    cur_open_bins--;
    return idx;
}

void Sim::update_volume(vector_3d box)
{
    total_volume += box.x * box.y * box.z;
}
void Sim::set_limits(int bins_limit, int open_bins_limit)
{
    max_bin_limit = bins_limit;
    max_open_limit = open_bins_limit;
}
int Sim::open_new_bin()
{
    if (bin_instances.size() >= max_bin_limit)
        return 0;
    Bin new_bin = Bin();
    bin_instances.push_back(new_bin);
    cur_open_bins++;
    if (cur_open_bins > max_open_limit)
    {
        close_bin();
    }
    return 1;
}
performance_metric Sim::get_performance_metric()
{
    int total_volume = 0;
    int no_of_bins_used = 0;
    int no_of_boxes_put = 0;
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
    performance_metric pm;
    pm.total_number_of_boxes = size_of_box_stream;
    pm.efficiency = efficiency;
    pm.number_of_bins_used = no_of_bins_used;
    pm.number_of_boxes_successfully_put = no_of_boxes_put;
    return pm;
}

int Sim::step(int bin_id, int icp_bcp_list_id, vector_3d box, int orientation)
{
    auto icp_bcp_list = bin_instances[bin_id].get_icbp_list();
    if (orientation == 0)
    {
        if (bin_instances[bin_id].update_icpbcp_list(icp_bcp_list_id, box) && bin_instances[bin_id].update_state({icp_bcp_list[icp_bcp_list_id].first.x, icp_bcp_list[icp_bcp_list_id].first.y}, box))
        {
            total_volume += box.x * box.y * box.z;
            total_number_of_boxes_placed++;
            bin_instances[bin_id].volume += box.x * box.y * box.z;
            bin_instances[bin_id].no_of_boxes_placed++;
            return 1;
        }
        else
        {
            std::cout << "exception occured in step" << std::endl;
            return 0;
        }
    }
    else
    {
        if (bin_instances[bin_id].update_icpbcp_list(icp_bcp_list_id, {box.y, box.x, box.z}) && bin_instances[bin_id].update_state({icp_bcp_list[icp_bcp_list_id].first.x, icp_bcp_list[icp_bcp_list_id].first.y}, {box.y, box.x, box.z}))
        {
            total_volume += box.x * box.y * box.z;
            total_number_of_boxes_placed++;
            bin_instances[bin_id].volume += box.x * box.y * box.z;
            bin_instances[bin_id].no_of_boxes_placed++;
            return 1;
        }
        else
        {
            std::cout << "exception occured in step" << std::endl;
            return 0;
        }
    }
}
int Sim::step(int bin_id, std::pair<int, int> position, vector_3d box, int orientation)
{
    auto &bin_instance = bin_instances[bin_id];
    if (orientation == 0)
    {
        if (bin_instance.update_state(position, box))
        {
            total_volume += box.x * box.y * box.z;
            total_number_of_boxes_placed++;
            bin_instances[bin_id].volume += box.x * box.y * box.z;
            bin_instances[bin_id].no_of_boxes_placed++;
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
        if (bin_instance.update_state(position, {box.y, box.x, box.z}))
        {
            total_volume += box.x * box.y * box.z;
            total_number_of_boxes_placed++;
            bin_instances[bin_id].volume += box.x * box.y * box.z;
            bin_instances[bin_id].no_of_boxes_placed++;
            return 1;
        }
        else
        {
            std::cout << "exception occured" << std::endl;
            return 0;
        }
    }
}