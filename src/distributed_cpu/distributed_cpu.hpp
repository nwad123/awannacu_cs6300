#pragma once

#include <vector>

auto Get_arg(int argc, char** argv, const int rank) -> std::tuple<int, int, int>;

auto calculateVisibilityLocal(
    const std::vector<int16_t>& height_map, 
    const int width, const int height, 
    const int start_y, const int end_y, const int rank,
    const int radius, const int num_angles) -> std::vector<unsigned int>;