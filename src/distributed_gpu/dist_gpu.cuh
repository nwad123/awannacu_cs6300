#pragma once

#include <cstdint>
#include <cuda_runtime.h>
#include <vector>

__global__ void calculate_visibility_kernel(
    const int16_t *height_map,
    unsigned int *visibility_map,
    int width,
    int height,
    int radius,
    int num_angles,
    float *ray_directions_x,
    float *ray_directions_y
);

std::vector<unsigned int> calculate_visibility_cuda(
    const std::vector<int16_t> &height_map,
    size_t width,
    size_t height,
    int radius,
    int angle,
    const int start_y, 
    const int end_y, 
    const int my_rank
);
