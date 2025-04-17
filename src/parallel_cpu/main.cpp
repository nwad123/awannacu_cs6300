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

#ifdef _OPENMP 
    #include <omp.h>
#endif

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
    const size_t width = std::stoul(argv[3]);
    const size_t height = std::stoul(argv[4]);
	const size_t angle = std::stoul(argv[5]);
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