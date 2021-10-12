#include "sim.h"

Sim::Sim()
{
    max_bin_limit = 1;
    max_open_limit = 1;
    total_volume=0;
    cur_open_bins=0;
    total_number_of_boxes_placed=0;
    bin_instances=std::vector<Bin>();
    size_of_box_stream=0;


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
    //std::cout<<"binstate";
    //new_bin.print_state();
    bin_instances.push_back(new_bin);
    cur_open_bins++;
    if (cur_open_bins > max_open_limit)
    {
        close_bin();
    }
    return 1;
}
performance_metric Sim::get_performance_metric(int calculate_open_bin_efficiency)
{
    int total_volume = 0;
    int no_of_bins_used = 0;
    int no_of_boxes_put = 0;
    for (int i = 0; i < bin_instances.size(); i++)
    {
        if (calculate_open_bin_efficiency || !bin_instances[i].is_open()) {
            total_volume += bin_instances[i].volume;
            no_of_bins_used++;
            no_of_boxes_put += bin_instances[i].no_of_boxes_placed;
        }
    }
    double efficiency = total_volume / (double)(no_of_bins_used * BIN_WIDTH * BIN_HEIGHT * BIN_LENGTH);
    //std::cout<<efficiency<<" "<<total_volume<<" "<<no_of_bins_used<<" "<< no_of_boxes_put<<" "<<size_of_box_stream<<std::endl;
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
        auto pos=icp_bcp_list[icp_bcp_list_id];
        auto start_point=get_start_point(pos, orientation, box);
        int x=start_point.first,y=start_point.second;
        int height=bin_instances[bin_id].update_state({x, y}, box);
        if ( height!=-1 && bin_instances[bin_id].update_icpbcp_list(pos, box)!=-1)
        {
            total_volume += box.x * box.y * box.z;
            total_number_of_boxes_placed++;
            bin_instances[bin_id].volume += box.x * box.y * box.z;
            bin_instances[bin_id].no_of_boxes_placed++;
            return height;
        }
        else
        {
            std::cout << "exception occured in step" << std::endl;
            return -1;
        }
    }
    else
    {
        auto pos=icp_bcp_list[icp_bcp_list_id];
        auto start_point=get_start_point(pos, orientation, box);
        int x=start_point.first,y=start_point.second;
        int height=bin_instances[bin_id].update_state({x, y}, {box.y, box.x, box.z});
        if (height!=-1 && bin_instances[bin_id].update_icpbcp_list(pos, {box.y, box.x, box.z})!=-1)
        {
            total_volume += box.x * box.y * box.z;
            total_number_of_boxes_placed++;
            bin_instances[bin_id].volume += box.x * box.y * box.z;
            bin_instances[bin_id].no_of_boxes_placed++;
            return height;
        }
        else
        {
            std::cout << "exception occured in step" << std::endl;
            return -1;
        }
    }
}
int Sim::step(int bin_id, std::pair<int, int> position, vector_3d box, int orientation)
{
    auto &bin_instance = bin_instances[bin_id];
    if (orientation == 0)
    {
        int height=bin_instance.update_state(position, box);
        if (height!=-1)
        {
            total_volume += box.x * box.y * box.z;
            total_number_of_boxes_placed++;
            bin_instances[bin_id].volume += box.x * box.y * box.z;
            bin_instances[bin_id].no_of_boxes_placed++;
            return height;
        }
        else
        {
            std::cout << "exception occured" << std::endl;
            return -1;
        }
    }
    else
    {   
        int height=bin_instance.update_state(position, {box.y, box.x, box.z});
        if (height!=-1)
        {
            total_volume += box.x * box.y * box.z;
            total_number_of_boxes_placed++;
            bin_instances[bin_id].volume += box.x * box.y * box.z;
            bin_instances[bin_id].no_of_boxes_placed++;
            return height;
        }
        else
        {
            std::cout << "exception occured" << std::endl;
            return -1;
        }
    }
}