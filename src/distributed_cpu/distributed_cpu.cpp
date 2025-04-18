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
            
            unsigned short current_height = height_map[y * width + x];
            
            // Start the count at this cell as 1 (the pixel itself is always visible)
            unsigned int visible_count = 1;
            
            // Cast rays in different directions
            for (const auto& [dx, dy] : ray_directions) {
                // Use a more efficient ray casting approach
                // Start from the center and move outward
                float max_angle_seen = std::numeric_limits<float>::lowest();
                
                // Step size for more efficient ray traversal
                const float ray_length = std::sqrt(dx*dx + dy*dy);
                const float step_x = dx / ray_length;
                const float step_y = dy / ray_length;
                
                // Start at center of pixel for our floating point values
                float curr_x_f = x + 0.5f;
                float curr_y_f = y + 0.5f;
                                
                while(true) {
                    // Move along the ray
                    curr_x_f += step_x;
                    curr_y_f += step_y;
                    
                    // Round to nearest pixel
                    const int curr_x = std::round(curr_x_f);
                    const int curr_y = std::round(curr_y_f);
                    
                    // Check bounds
                    if (curr_x < 0 || curr_x >= width || curr_y < 0 || curr_y >= height)
                        break;
                    
                    // Check if we've gone too far (outside the RADIUS)
                    int dist_squared = (curr_x - x)*(curr_x - x) + (curr_y - y)*(curr_y - y);
                    if (dist_squared > radius_squared)
                        break;
                    
                    // Get height at current position
                    unsigned short point_height = height_map[curr_y * width + curr_x];
                    
                    // Calculate angle to determine visibility
                    float distance = std::sqrt(dist_squared);
                    float height_diff = point_height - current_height;
                    float angle = height_diff / distance;
                    
                    // If the angle is greater than the maximum seen so far,
                    // the pixel is visible
                    if (angle > max_angle_seen) {
                        max_angle_seen = angle;
                        visible_count++;
                    }
                }
            }
            
            // Store the visibility count in the local map
            local_visibility[(y - start_y) * width + x] = visible_count;
        }
    }

    if(rank == 0)
        std::cout << "\r100% Complete" << std::endl;

    return local_visibility;
}