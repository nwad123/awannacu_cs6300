#include "parallel_cpu.hpp"
#include <fmt/core.h>
#include <algorithm>
#include <iostream>

#ifdef _OPENMP 
#include <omp.h>
#endif

auto calculateVisibility(const std::vector<int16_t>& height_map, 
                         size_t width, size_t height, 
                         int radius, int angle) -> std::vector<unsigned int>
{
    std::vector<unsigned int> visibility_map(width * height, 0);
    const int radius_squared = radius * radius;
    
    // Number of discrete angles
    const int num_angles = std::abs(angle); 
    // The distance between each angle in radians
    const double angle_step = 2 * M_PI / num_angles;
    
    std::vector<std::pair<float, float>> ray_directions;
    ray_directions.reserve(static_cast<uint64_t>(num_angles));
    
    // precalculate the angle of the rays to be cast
    for (int i = 0; i < num_angles; ++i) {
        const double angle_ = i * angle_step;
        float dx = std::round(std::cos(angle_) * radius);
        float dy = std::round(std::sin(angle_) * radius);
        ray_directions.push_back({dx, dy});
    }
    
    // Process each pixel
    //use parallel cpu with opeMP
#pragma omp parallel for collapse(2)
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            visibility_map[y * width + x] = single_pixel_visiblity(
                x, y, width, height, radius, radius_squared, height_map, ray_directions
            );
        }
    }

    return visibility_map;
}