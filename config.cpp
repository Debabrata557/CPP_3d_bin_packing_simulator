#include "config.h"

bool operator<(const vector_3d &a, const vector_3d &b)
{
    if (a.x != b.x)
    {
        return a.x < b.x;
    }
    else
    {
        if (a.y != b.y)
        {
            return a.y < b.y;
        }
        else
        {
            return a.z < b.z;
        }
    }
}

bool comp_floor_building(std::pair<vector_3d, vector_3d> &a, std::pair<vector_3d, vector_3d> &b)
{
    std::pair<vector_3d, vector_3d> temp_a = {{a.first.z, a.first.x, a.first.y}, {a.second.z, a.second.x, a.second.y}};
    std::pair<vector_3d, vector_3d> temp_b = {{b.first.z, b.first.x, b.first.y}, {b.second.z, b.second.x, b.second.y}};
    return temp_a < temp_b;
}

bool comp_first_fit(std::pair<vector_3d, vector_3d> &a, std::pair<vector_3d, vector_3d> &b)
{
    return a < b;
}

void print_vector_3d(vector_3d &a)
{
    std::cout << " x: " << a.x << " y: " << a.y << " z: " << a.z << std::endl;
}
void print_list_vector_3d(std::vector<vector_3d> &a)
{
    int n = a.size();
    for (int i = 0; i < n; i++)
    {
        print_vector_3d(a[i]);
    }
}
void print_pair_vector_3d(std::pair<vector_3d, vector_3d> &a)
{
    std::cout << " x1: " << a.first.x << " y1: " << a.first.y << " z1: " << a.first.z << "  "
              << " x2: " << a.second.x << " y2: " << a.second.y << " z2: " << a.second.z << std::endl;
}
void print_list_pair_vector_3d(std::vector<std::pair<vector_3d, vector_3d>> &a)
{
    int n = a.size();
    for (int i = 0; i < n; i++)
    {
        print_pair_vector_3d(a[i]);
    }
}
void print_state(std::vector<std::vector<int>> &a){
    for (int i=0;i<a.size();i++){
        for(int j=0;j<a[0].size();j++){
            std::cout<<a[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
}