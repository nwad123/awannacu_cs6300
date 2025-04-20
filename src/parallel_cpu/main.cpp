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

// #ifdef _OPENMP 
    #include <omp.h>
// #endif

int main(int argc, char** argv) {
    // Usage: ./<exec> <read_file> <write_file> <width> <height>
    if (argc != 7) {
        std::cerr << "Usage: " << argv[0] << " <read_file> <write_file> <width> <height> <angle> <threads>" << std::endl;
        return 1;
    }
    
// #ifdef _OMP
    // set the number of threads to use
    const auto num_threads = std::stoi(argv[6]);
    omp_set_num_threads(num_threads);
    fmt::println("Set number of threads to {}", num_threads);
// #endif

    // Parse width and height from command line
    const size_t width = std::stoul(argv[3]);
    const size_t height = std::stoul(argv[4]);
	const int angle = std::stoi(argv[5]);
    
    // Allocate memory for height map
    std::vector<int16_t> height_map = read_input(argv[1]);
    std::cout << "Height map loaded: " << width << "x" << height << std::endl;
    
    // time the algorithm
    timer time;
    time.reset();

    // Calculate visibility map
    int radius = 100;
    std::vector<uint32_t> visibility_map = calculateVisibility(height_map, width, height, radius, angle);

    // display the elapsed time
    fmt::println("Elapsed time: {} ms", time.read());
    
    // Write the output
    write_output<uint32_t>(argv[2], visibility_map);
    std::cout << "Output written to: " << argv[2] << std::endl;
    
    return 0;
}