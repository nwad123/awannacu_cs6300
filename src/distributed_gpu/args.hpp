#pragma once

#include <vector>
#include <cstdint>
#include <tuple>

/// @brief Parses the command line arguments
/// @param argc argc from main
/// @param argv argv from main
/// @param rank rank from MPI
/// @return [width, height, angle] if any are invalid they will be less than
///         or equal to zero.
auto Get_arg(int argc, char** argv, const int rank) -> std::tuple<int, int, int>;