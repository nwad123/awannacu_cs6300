// This file will use the MPI library to distribute the work of the Bresenham line algorithm across multiple processes.
// It will read in a height map (16 bit raw) and find the output
// The output will be a 32bit file which will detect the number of visible pixels from every other cell.
// The maximum distance will be a RADIUS of 100 pixels.
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

#include "distributed_cpu.hpp"
#include <mpi.h>

int my_rank, comm_sz;
MPI_Comm comm;

#define RADIUS 100

// Function to get command line arguments
void Get_arg(int argc, char** argv, int* width, int* height, int* angle);

// Function to calculate visibility for a portion of the map
std::vector<unsigned int> calculateVisibilityLocal(
    const std::vector<unsigned short>& height_map, 
    int width, int height, 
    int start_y, int end_y,
    int radius, int num_angles) {
    
    std::vector<unsigned int> local_visibility(width * (end_y - start_y), 0);
    const int radius_squared = RADIUS * RADIUS;
    
    const double angle_step = 2 * M_PI / num_angles;
    
    std::vector<std::pair<int, int>> ray_directions;
    ray_directions.reserve(num_angles);
    
    for (int i = 0; i < num_angles; ++i) {
        double angle = i * angle_step;
        int dx = static_cast<int>(std::round(std::cos(angle) * RADIUS));
        int dy = static_cast<int>(std::round(std::sin(angle) * RADIUS));
        ray_directions.push_back({dx, dy});
    }
    
    // Process each pixel in assigned range
    for (int y = start_y; y < end_y; ++y) {
        for (int x = 0; x < width; ++x) {
            // Print progress more frequently
            if ((y * width + x) % 1000 == 0 && my_rank == 0) {
                std::cout << "\r" << (static_cast<float>((y - start_y) * width + x) / (width * (end_y - start_y))) * 100 << "%";
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
                int max_steps = RADIUS;
                int curr_x = x;
                int curr_y = y;
                float max_angle_seen = -std::numeric_limits<float>::infinity();
                
                // Step size for more efficient ray traversal
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
                    if (curr_x < 0 || curr_x >= width || curr_y < 0 || curr_y >= height)
                        break;
                    
                    // Check if we've gone too far (outside the RADIUS)
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
            
            // Store the visibility count in the local map
            local_visibility[(y - start_y) * width + x] = visible_count;
        }
    }
    if(my_rank == 0)
        std::cout << "\r100\% Complete" << std::endl;
    return local_visibility;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    // MPI specific initialization
    int initialized;
    MPI_Initialized(&initialized);
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    // Parse command line arguments
    int width, height, angle;
    Get_arg(argc, argv, &width, &height, &angle);
       
    // Only rank 0 reads the input file
    std::vector<unsigned short> height_map;
    
    if (my_rank == 0) {
        height_map.resize(width * height);
        
        // Double check inputs
        printf("Parameters: width=%d, height=%d, angle=%d\n", width, height, angle);
        
        // Open input file
        std::ifstream input_file(argv[1], std::ios::binary);
        if (!input_file) {
            std::cerr << "Error opening input file: " << argv[1] << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        
        int expected_size = width * height * sizeof(unsigned short);
        
        // Get file size
        input_file.seekg(0, std::ios::end);
        std::streamsize file_size = input_file.tellg();
        input_file.seekg(0, std::ios::beg);
        
        // Verify file size matches expected dimensions
        if (file_size != expected_size) {
            std::cerr << "Error: File size (" << file_size << " bytes) doesn't match expected dimensions: " 
                    << width << "x" << height << " (" << expected_size << " bytes)" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        
        // Read the file directly into the vector
        input_file.read(reinterpret_cast<char*>(height_map.data()), file_size);
        input_file.close();
        
        std::cout << "Height map loaded: " << width << "x" << height << std::endl;
    }
    
    // Broadcast height map dimensions to all processes
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Resize height_map vector on all non-root processes
    if (my_rank != 0) {
        height_map.resize(width * height);
    }
    
    // Broadcast the entire height map to all processes
    MPI_Bcast(height_map.data(), width * height, MPI_UNSIGNED_SHORT, 0, MPI_COMM_WORLD);
    
    // Initialize visibility map on root process
    std::vector<unsigned int> visibility_map;
    if (my_rank == 0) {
        visibility_map.resize(width * height, 0);
    }
    
    // Divide work by rows
    int rows_per_proc = height / comm_sz;
    int remaining_rows = height % comm_sz;
    
    // Calculate start and end rows for each process
    int start_row = my_rank * rows_per_proc + std::min(my_rank, remaining_rows);
    int end_row = start_row + rows_per_proc + (my_rank < remaining_rows ? 1 : 0);

    // Calculate local visibility
    std::vector<unsigned int> local_visibility = calculateVisibilityLocal(
        height_map, width, height, start_row, end_row, RADIUS, angle);
    
    // Prepare for gathering results
    std::vector<int> recv_counts;
    std::vector<int> displacements;
    
    //manually divide the work into rows,
    //because scatterv wasn't working :'( 
    //gatherv does, though... so you know.
    if (my_rank == 0) {
        recv_counts.resize(comm_sz);
        displacements.resize(comm_sz);
        
        int disp = 0;
        for (int i = 0; i < comm_sz; i++) {
            int proc_rows = rows_per_proc + (i < remaining_rows ? 1 : 0);
            recv_counts[i] = proc_rows * width;
            displacements[i] = disp;
            disp += recv_counts[i];
        }
    }
    
    // Gather all local visibility results to the root process
    MPI_Gatherv(
        local_visibility.data(), local_visibility.size(), MPI_UNSIGNED,
        visibility_map.data(), recv_counts.data(), displacements.data(), MPI_UNSIGNED,
        0, MPI_COMM_WORLD
    );
    
    // Write output
    if (my_rank == 0) {
        std::ofstream output_file(argv[2], std::ios::binary);
        if (!output_file) {
            std::cerr << "Error opening output file: " << argv[2] << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        
        output_file.write(reinterpret_cast<const char*>(visibility_map.data()), 
                        visibility_map.size() * sizeof(unsigned int));
        output_file.close();
        
        std::cout << "Output written to: " << argv[2] << std::endl;
    }
    
    MPI_Finalize();
    return 0;
}

void Get_arg(int argc, char** argv, int* width, int* height, int* angle) {
    if (my_rank == 0) {
        if (argc == 6) {
            *width = std::stoi(argv[3]);
            *height = std::stoi(argv[4]);
            *angle = std::stoi(argv[5]);
        } else {
            std::cerr << "Usage: " << argv[0] << " <read_file> <write_file> <width> <height> <angle>" << std::endl;
            *angle = -1;
        }
    }
    
    // Broadcast all parameters
    MPI_Bcast(width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(angle, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (*angle <= 0) {
        // Kill program if usage isn't correct
        MPI_Finalize();
        exit(0);
    }
}