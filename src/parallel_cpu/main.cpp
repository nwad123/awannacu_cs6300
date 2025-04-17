// This file will use the MPI library to distribute the work of the Bresenham line algorithm across multiple processes.
// It will read in a hight map (16 bit raw) and find the output
// The output will be a 32bit file which will detect the number of visible pixels from every other cell.
// The maximum distance will be a radius of 100 pixels.
// each pixel in the output will be the number of visible pixels from that pixel

#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <stdio.h>
#include <algorithm> // For std::max
#include <iterator> // For std::istreambuf_iterator
#include <cstdlib> // For std::abs
#include <cstring> // For std::memcpy

#include "parallel_cpu.hpp"
// #include "core.hpp"
// #include <fmt/core.h>

#ifdef _OPENMP 
    #include <omp.h>
#endif



std::vector<unsigned int> calculateVisibility(const std::vector<unsigned short>& height_map, size_t width, size_t height, int radius = 100, int angle = 12) {
    std::vector<unsigned int> visibility_map(width * height, 0);
    const int radius_squared = radius * radius;
    
    const int num_angles = angle;  // Number of discrete angles
    const double angle_step = 2 * M_PI / num_angles;
    
    std::vector<std::pair<int, int>> ray_directions;
    ray_directions.reserve(num_angles);
    
    for (int i = 0; i < num_angles; ++i) {
        double angle = i * angle_step;
        int dx = static_cast<int>(std::round(std::cos(angle) * radius));
        int dy = static_cast<int>(std::round(std::sin(angle) * radius));
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
            // Skip walls
            if (current_height == 0) continue;
            
            // Start with 1 (the pixel itself is always visible)
            unsigned int visible_count = 1;
            
            // Cast rays in different directions
            for (const auto& [dx, dy] : ray_directions) {
                // Use a more efficient ray casting approach
                // Start from the center and move outward
                int max_steps = radius;
                int curr_x = x;
                int curr_y = y;
                float max_angle_seen = -std::numeric_limits<float>::infinity();
                
                // Step size for more efficient ray traversal
                // For long rays, we don't need to check every pixel
                const float ray_length = std::sqrt(dx*dx + dy*dy);
                const float step_x = dx / ray_length;
                const float step_y = dy / ray_length;
                
                float curr_x_f = x + 0.5f;  // Start at center of pixel
                float curr_y_f = y + 0.5f;
                
                bool hit_wall = false;
                
                for (int step = 1; step <= max_steps; ++step) {
                    // Move along the ray
                    curr_x_f += step_x;
                    curr_y_f += step_y;
                    
                    // Round to nearest pixel
                    curr_x = std::round(curr_x_f);
                    curr_y = std::round(curr_y_f);
                    
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
                    
                    // Skip if it's a wall (height 0)
                    if (point_height == 0) {
                        hit_wall = true;
                        break;
                    }
                    
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
    
    std::cout << "\rProgress: 100%" << std::endl;
    return visibility_map;
}

int main(int argc, char** argv) {
    // Usage: ./<exec> <read_file> <write_file> <width> <height>
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <read_file> <write_file> <width> <height> <angle> <threads>" << std::endl;
        return 1;
    }
    
#ifdef _OMP
    omp_set_num_threads(std::stoi(argv[6]));
#endif
    // Parse width and height from command line
    int width = std::stoi(argv[3]);
    int height = std::stoi(argv[4]);
	int angle = std::stoi(argv[5]);
    size_t expected_size = width * height * sizeof(unsigned short);
    
    // Open input file
    std::ifstream input_file(argv[1], std::ios::binary);
    if (!input_file) {
        std::cerr << "Error opening input file: " << argv[1] << std::endl;
        return 1;
    }
    
    // Get file size
    input_file.seekg(0, std::ios::end);
    std::streamsize file_size = input_file.tellg();
    input_file.seekg(0, std::ios::beg);
    
    // Verify file size matches expected dimensions
    if (file_size != expected_size) {
        std::cerr << "Error: File size (" << file_size << " bytes) doesn't match expected dimensions: " 
                  << width << "x" << height << " (" << expected_size << " bytes)" << std::endl;
        return 1;
    }
    
    // Allocate memory for height map
    std::vector<unsigned short> height_map(width * height);
    
    // Read the file directly into the vector
    input_file.read(reinterpret_cast<char*>(height_map.data()), file_size);
    input_file.close();
    
    std::cout << "Height map loaded: " << width << "x" << height << std::endl;
    
    // Calculate visibility map
    int radius = 100;
    std::vector<unsigned int> visibility_map = calculateVisibility(height_map, width, height, radius, angle);
    
    // Write output
    std::ofstream output_file(argv[2], std::ios::binary);
    if (!output_file) {
        std::cerr << "Error opening output file: " << argv[2] << std::endl;
        return 1;
    }
    
    output_file.write(reinterpret_cast<const char*>(visibility_map.data()), 
                      visibility_map.size() * sizeof(unsigned int));
    output_file.close();
    
    std::cout << "Output written to: " << argv[2] << std::endl;
    
    return 0;
}