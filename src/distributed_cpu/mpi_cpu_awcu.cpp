#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

// Structure for a 2D point
struct Point {
    int x, y;
};

std::vector<unsigned int> calculateVisibilityMPI(const std::vector<unsigned short>& height_map, 
                                               size_t width, size_t height, int radius = 100) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const size_t total_pixels = width * height;
    std::vector<unsigned int> local_visibility_map;
    std::vector<unsigned int> global_visibility_map;
    
    // Only root process needs to allocate full map size
    if (rank == 0) {
        global_visibility_map.resize(total_pixels, 0);
    }
    
    // Calculate workload distribution
    size_t pixels_per_process = total_pixels / size;
    size_t start_pixel = rank * pixels_per_process;
    size_t end_pixel = (rank == size - 1) ? total_pixels : (rank + 1) * pixels_per_process;
    
    // Allocate local visibility map
    local_visibility_map.resize(end_pixel - start_pixel, 0);

    // Pre-compute radius squared for circle check
    const int radius_squared = radius * radius;
    
    // Pre-compute the circle points once
    std::vector<std::pair<int, int>> circle_offsets;
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            if (dx*dx + dy*dy <= radius_squared) {
                circle_offsets.push_back({dx, dy});
            }
        }
    }
    
    // Process assigned pixels
    for (size_t pixel_idx = start_pixel; pixel_idx < end_pixel; ++pixel_idx) {
        size_t x = pixel_idx % width;
        size_t y = pixel_idx / width;
        
        // Print progress on each process (less frequently to reduce output clutter)
        if (pixel_idx % 1000 == 0) {
            std::cout << "Rank " << rank << " progress: " 
                      << (static_cast<float>(pixel_idx - start_pixel) / (end_pixel - start_pixel)) * 100 
                      << "%" << std::endl;
        }
        
        unsigned short current_height = height_map[y * width + x];
        // Skip walls or invalid pixels
        if (current_height == 0) continue;
        
        // Count visible points from this location
        unsigned int visible_count = 0;
        
        // Check each point in the circle
        for (const auto& [dx, dy] : circle_offsets) {
            int target_x = static_cast<int>(x) + dx;
            int target_y = static_cast<int>(y) + dy;
            
            // Skip if out of bounds
            if (target_x < 0 || target_x >= static_cast<int>(width) || 
                target_y < 0 || target_y >= static_cast<int>(height))
                continue;
            
            // Skip walls
            unsigned short target_height = height_map[target_y * width + target_x];
            if (target_height == 0) continue;
            
            // Skip the center pixel itself (already counted)
            if (dx == 0 && dy == 0) {
                visible_count++;
                continue;
            }
            
            // Check line of sight using Bresenham's algorithm
            bool is_visible = true;
            
            // Plot line and check occlusion
            int x0 = x, y0 = y;
            int x1 = target_x, y1 = target_y;
            
            bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);
            if (steep) {
                std::swap(x0, y0);
                std::swap(x1, y1);
            }
            
            if (x0 > x1) {
                std::swap(x0, x1);
                std::swap(y0, y1);
            }
            
            int dx_line = x1 - x0;
            int dy_line = std::abs(y1 - y0);
            int error = dx_line / 2;
            int ystep = (y0 < y1) ? 1 : -1;
            int y_line = y0;
            
            // Keep track of maximum height encountered along the line
            float max_angle = -std::numeric_limits<float>::infinity();
            
            for (int x_line = x0; x_line <= x1; ++x_line) {
                int curr_x = steep ? y_line : x_line;
                int curr_y = steep ? x_line : y_line;
                
                // Skip checking the endpoints themselves
                if ((curr_x == static_cast<int>(x) && curr_y == static_cast<int>(y)) ||
                    (curr_x == target_x && curr_y == target_y)) {
                    error -= dy_line;
                    if (error < 0) {
                        y_line += ystep;
                        error += dx_line;
                    }
                    continue;
                }
                
                // Get the height at this point
                unsigned short point_height = height_map[curr_y * width + curr_x];
                
                // Skip walls in line of sight calculation
                if (point_height == 0) {
                    is_visible = false;
                    break;
                }
                
                // Calculate the angle to this point
                float distance = std::sqrt(
                    std::pow(curr_x - static_cast<int>(x), 2) + 
                    std::pow(curr_y - static_cast<int>(y), 2)
                );
                
                if (distance > 0) {
                    float height_diff = point_height - current_height;
                    float angle = height_diff / distance;
                    
                    if (angle > max_angle) {
                        max_angle = angle;
                    } else if (point_height > current_height) {
                        // If the point is higher than current but has a lower angle,
                        // it means it's obscured by a closer point
                        is_visible = false;
                        break;
                    }
                }
                
                error -= dy_line;
                if (error < 0) {
                    y_line += ystep;
                    error += dx_line;
                }
            }
            
            if (is_visible) {
                visible_count++;
            }
        }
        
        // Store the result in local map
        local_visibility_map[pixel_idx - start_pixel] = visible_count;
    }
    
    // Gather results to root process
    if (rank == 0) {
        // First copy local results to global map
        for (size_t i = 0; i < local_visibility_map.size(); i++) {
            global_visibility_map[start_pixel + i] = local_visibility_map[i];
        }
        
        // Receive results from other processes
        for (int src_rank = 1; src_rank < size; src_rank++) {
            size_t src_start = src_rank * pixels_per_process;
            size_t src_size = (src_rank == size - 1) ? 
                              (total_pixels - src_start) : pixels_per_process;
            
            std::vector<unsigned int> recv_buffer(src_size);
            MPI_Recv(recv_buffer.data(), src_size, MPI_UNSIGNED, 
                     src_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            // Copy received data to global map
            for (size_t i = 0; i < src_size; i++) {
                global_visibility_map[src_start + i] = recv_buffer[i];
            }
        }
    } else {
        // Send local results to root
        MPI_Send(local_visibility_map.data(), local_visibility_map.size(), 
                 MPI_UNSIGNED, 0, 0, MPI_COMM_WORLD);
    }
    
    // Return empty vector for non-root processes
    if (rank != 0) {
        return std::vector<unsigned int>();
    }
    
    return global_visibility_map;
}

// Function to use in main
std::vector<unsigned int> processHeightMap(const std::vector<unsigned short>& height_map, 
                                         size_t width, size_t height, int radius = 100) {
    int initialized, rank, size;
    MPI_Initialized(&initialized);
    
    if (!initialized) {
        // Initialize MPI if not already done
        int provided;
        MPI_Init_thread(NULL, NULL, MPI_THREAD_FUNNELED, &provided);
    }
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Print basic info about the run
    if (rank == 0) {
        std::cout << "Processing height map with " << size << " MPI processes" << std::endl;
        std::cout << "Height map dimensions: " << width << "x" << height << std::endl;
        std::cout << "Visibility radius: " << radius << std::endl;
    }
    
    // Broadcast the height map to all processes
    std::vector<unsigned short> local_height_map;
    
    if (rank == 0) {
        local_height_map = height_map;
    } else {
        local_height_map.resize(width * height);
    }
    
    MPI_Bcast(local_height_map.data(), width * height, MPI_UNSIGNED_SHORT, 0, MPI_COMM_WORLD);
    
    // Calculate visibility
    std::vector<unsigned int> visibility_map = calculateVisibilityMPI(
        local_height_map, width, height, radius);
    
    // Don't finalize MPI here to allow for other MPI operations in the program
    return visibility_map;
}

// Example main function showing usage
int main(int argc, char** argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Only root process loads the data
    std::vector<unsigned short> height_map;
    size_t width = 0, height = 0;
    
    if (rank == 0) {
        // Load your height map here
        // For example:
        width = 1000;  // Replace with actual width
        height = 1000; // Replace with actual height
        height_map.resize(width * height);
        
        // Fill height_map with data...
        // (Code to load data from file would go here)
    }
    
    // Broadcast dimensions to all processes
    MPI_Bcast(&width, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
    
    // Non-root processes now allocate memory
    if (rank != 0) {
        height_map.resize(width * height);
    }
    
    // Broadcast height map data to all processes
    MPI_Bcast(height_map.data(), width * height, MPI_UNSIGNED_SHORT, 0, MPI_COMM_WORLD);
    
    // Process the height map
    std::vector<unsigned int> visibility_map = processHeightMap(height_map, width, height, 100);
    
    // Only root process has the results
    if (rank == 0) {
        std::cout << "Calculation complete, writing results..." << std::endl;
        
        // Process visibility_map (e.g., write to file)
        // (Code to save results would go here)
    }
    
    // Finalize MPI
    MPI_Finalize();
    return 0;
}