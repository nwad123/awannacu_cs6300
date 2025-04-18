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
#pragma omp parallel for collapse(2) schedule(dynamic, 1)
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            // Print progress more frequently
            if ((y * width + x) % 1000 == 0) {
                std::cout << "\rProgress: " << (static_cast<float>(y * width + x) / (width * height)) * 100 << "%";
                std::cout.flush();
            }
            
            unsigned short current_height = height_map[y * width + x];
            
            // Start the count at this cell as 1 (the pixel itself is always visible)
            unsigned int visible_count = 1;
            
            // Cast rays in different directions
            for (const auto& [dx, dy] : ray_directions) {
                // Use a more efficient ray casting approach
                // Start from the center and move outward
                int max_steps = radius;
                float max_angle_seen = -std::numeric_limits<float>::infinity();
                
                // Step size for more efficient ray traversal
                // For long rays, we don't need to check every pixel
                const float ray_length = std::sqrt(dx*dx + dy*dy);
                const float step_x = dx / ray_length;
                const float step_y = dy / ray_length;

                float curr_x_f = x + 0.5f;  // Start at center of pixel
                float curr_y_f = y + 0.5f;
                
                for (int step = 1; step <= max_steps; ++step) {
                    // Move along the ray
                    curr_x_f += step_x;
                    curr_y_f += step_y;
                    
                    // Round to nearest pixel
                    const int curr_x = std::round(curr_x_f);
                    const int curr_y = std::round(curr_y_f);
                    
                    // Check bounds
                    if (curr_x < 0 || curr_x >= static_cast<int>(width) || 
                        curr_y < 0 || curr_y >= static_cast<int>(height))
                        break;
                    
                    // Check if we've gone too far (outside the radius)
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
            
            // Store the visibility count
            visibility_map[y * width + x] = visible_count;
        }
    }
    
    std::cout << "\rProgress: 100% " << std::endl;

    return visibility_map;
}