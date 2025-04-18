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
#include "core.hpp"
#include <mpi.h>

// Globals
int my_rank, comm_sz;
MPI_Comm comm;
constexpr int RADIUS = 100;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    // MPI specific initialization
    int initialized;
    MPI_Initialized(&initialized);
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    // Parse command line arguments
    auto [width, height, angle] = Get_arg(argc, argv, my_rank);

    // Broadcast all parameters across processes
    MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&angle, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Validate input arguments. Each process does this so that they all 
    // can exit if the arguments are invalid.
    if (width <= 0 || height <= 0 || angle <= 0) {
        if (my_rank == 0)
            std::cout << "Invalid input arguments, exiting." << std::endl;

        // If the arguments are inalid finish and return 1
        MPI_Finalize();
        return 1;
    }
       
    // Every proces gets it's own height map
    std::vector<int16_t> height_map;
    
    // Only rank 0 reads the input file
    if (my_rank == 0) {
        // Display inputs
        printf("Parameters: width=%d, height=%d, angle=%d\n", width, height, angle);
        
        // Read height map
        height_map = read_input(argv[1]);
        std::cout << "Height map loaded: " << width << "x" << height << std::endl;
    }
    
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
        height_map, width, height, start_row, end_row, my_rank, RADIUS, angle);
    
    // Prepare for gathering results
    std::vector<int> recv_counts;
    std::vector<int> displacements;
    
    // manually divide the work into rows,
    // because scatterv wasn't working :'( 
    // gatherv does, though... so you know.
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
        write_output<uint32_t>(argv[2], visibility_map);
        std::cout << "Output written to: " << argv[2] << std::endl;
    }
    
    // Finish and return 0
    MPI_Finalize();
    return 0;
}