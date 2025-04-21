#include "distributed_cpu.hpp"
#include <fmt/core.h>
#include <cmath>
#include <iostream>
#include <utility>

auto Get_arg(int argc, char** argv, const int rank) -> std::tuple<int, int, int> {
    // initial parameters to the error state
    int width{-1}, height{-1}, angle{-1};

    // if the rank is 0 then parse the arguments and print the usage if
    // the arguments are incorrect
    if (rank == 0) {
        if (argc == 6) {
            width = std::stoi(argv[3]);
            height = std::stoi(argv[4]);
            angle = std::stoi(argv[5]);
        } else {
            std::cerr << "Usage: " << argv[0] << " <read_file> <write_file> <width> <height> <angle>" << std::endl;
        }
    }
    
    return {width, height, angle};
}

// Function to calculate visibility for a portion of the map
auto calculateVisibilityLocal(
    const std::vector<int16_t>& height_map, 
    const int width, const int height, 
    const int start_y, const int end_y, const int rank,
    const int radius, const int num_angles) -> std::vector<unsigned int> {
    
    std::vector<unsigned int> local_visibility(width * (end_y - start_y), 0);
    const int radius_squared = radius * radius;
    
    // the distance in radians between reach angle
    const double angle_step = 2 * M_PI / num_angles;
    
    std::vector<std::pair<float, float>> ray_directions;
    ray_directions.reserve(num_angles);
    
    // precalculate the angle of the rays to be cast
    for (int i = 0; i < num_angles; ++i) {
        double angle = i * angle_step;
        float dx = std::cos(angle) * radius;
        float dy = std::sin(angle) * radius;
        ray_directions.push_back({dx, dy});
    }
    
    // Process each pixel in assigned range
    for (int y = start_y; y < end_y; ++y) {
        for (int x = 0; x < width; ++x) {
            // Print progress more frequently
            if ((y * width + x) % 1000 == 0 && rank == 0) {
                std::cout << "\r" << (static_cast<float>((y - start_y) * width + x) / (width * (end_y - start_y))) * 100 << "%";
                std::cout.flush();
            }
            
            // Store the visibility count in the local map
            local_visibility[(y - start_y) * width + x] = single_pixel_visiblity(
                x, y, width, height, radius, radius_squared, height_map, ray_directions
            );
        }
    }

    if(rank == 0)
        std::cout << "\r100% Complete" << std::endl;

    return local_visibility;
}