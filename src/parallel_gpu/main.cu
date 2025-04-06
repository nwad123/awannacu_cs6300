#include "core.hpp"
#include "parallel_gpu.cuh"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cuda_runtime.h>
#include <fmt/core.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

int main(int argc, char **argv)
{

    std::vector<int16_t> height_map = read_input(argv[1]);
    const size_t width = std::stoul(argv[3]);
    const size_t height = std::stoul(argv[4]);
    const int angle = std::stoi(argv[5]);

    timer time;
    time.reset();

    int radius = 100;
    std::vector<unsigned int> visibility_map = calculate_visibility_cuda(height_map, width, height, radius, angle);

    fmt::println("Elapsed time: {} ms", time.read());

    write_output<unsigned int>(argv[2], visibility_map);

    return 0;
}
