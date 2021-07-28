#include "generate_box.h"

GenerateBox::GenerateBox(/* args */)
{
    max_size = {MAX_BOX_WIDTH, MAX_BOX_LENGTH, MAX_BOX_HEIGHT};
    min_size = {MIN_BOX_WIDTH, MIN_BOX_LENGTH, MIN_BOX_HEIGHT};
    // scaled_max_box_length = MAX_BOX_LENGTH * SCALING_FACTOR;
    // scaled_max_box_height = MAX_BOX_HEIGHT * SCALING_FACTOR;
    // scaled_max_box_width = MAX_BOX_WIDTH * SCALING_FACTOR;
    // scaled_min_box_length = MIN_BOX_LENGTH * SCALING_FACTOR;
    // scaled_min_box_height = MIN_BOX_HEIGHT * SCALING_FACTOR;
    // scaled_min_box_width = MIN_BOX_WIDTH * SCALING_FACTOR;
}
GenerateBox::~GenerateBox()
{
}

bool GenerateBox::is_valid_item(std::vector<int> &item)
{
    for (int i = 0; i < 3; i++)
    {
        if (item[i] < min_size[i] || item[i] > max_size[i])
            return false;
    }
    return true;
}
int GenerateBox::generate_cut1()
{
    try
    {
        std::vector<std::vector<int>> invalid;
        invalid.push_back(std::vector<int>({BIN_WIDTH, BIN_LENGTH, BIN_HEIGHT, 0, 0, 0}));
        while (invalid.size() > 0)
        {
            int invalidItemIndex = rand() % invalid.size();
            std::vector<int> invalidItem = invalid[invalidItemIndex];
            invalid.erase(invalid.begin() + invalidItemIndex);
            std::vector<int> axis;
            if (invalidItem[0] > max_size[0])
                axis.push_back(0);
            if (invalidItem[1] > max_size[1])
                axis.push_back(1);
            if (invalidItem[2] > max_size[2])
                axis.push_back(2);
            int axisChosen = axis[rand() % axis.size()];
            int cutLen = rand() % (invalidItem[axisChosen] - 2 * min_size[axisChosen]);
            std::vector<int> item1 = {invalidItem[0], invalidItem[1], invalidItem[2], invalidItem[3], invalidItem[4], invalidItem[5]};
            std::vector<int> item2 = {invalidItem[0], invalidItem[1], invalidItem[2], invalidItem[3], invalidItem[4], invalidItem[5]};
            item1[axisChosen] = min_size[axisChosen] + cutLen;
            item2[axisChosen] = invalidItem[axisChosen] - item1[axisChosen];
            item2[3 + axisChosen] = item2[3 + axisChosen] + item1[axisChosen];
            if (is_valid_item(item1))
                stream_of_boxes.push_back({item1[0], item1[1], item1[2]});
            else
                invalid.push_back(item1);
            if (is_valid_item(item2))
                stream_of_boxes.push_back({item2[0], item2[1], item2[2]});
            else
                invalid.push_back(item2);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 0;
    }

    return 1;
}

void GenerateBox::generate_cut2()
{
}
std::vector<vector_3d> GenerateBox::get_stream_of_boxes()
{
    return stream_of_boxes;
}