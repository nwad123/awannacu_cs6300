#pragma once

#include <vector>
#include <cstdint>
#include <tuple>
#include "core.hpp"

/// @brief Parses the command line arguments
/// @param argc argc from main
/// @param argv argv from main
/// @param rank rank from MPI
/// @return [width, height, angle] if any are invalid they will be less than
///         or equal to zero.
auto Get_arg(int argc, char** argv, const int rank) -> std::tuple<int, int, int>;

/// @brief Calculates the visible of a portion of the map
/// @param height_map the global height map
/// @param width the width of the global height map
/// @param height the height of the global height map
/// @param start_y the y-value (row) to start on for this process
/// @param end_y the y-value (row) to end for this process
/// @param rank the rank of this process
/// @param radius the radius of the circle to calculate
/// @param num_angles the number of angles (rays) to cast
/// @return the local visibility map for this process
auto calculateVisibilityLocal(
    const std::vector<int16_t>& height_map, 
    const int width, const int height, 
    const int start_y, const int end_y, const int rank,
    const int radius, const int num_angles) -> std::vector<unsigned int>;