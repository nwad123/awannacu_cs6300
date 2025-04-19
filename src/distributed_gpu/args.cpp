#include "args.hpp"

#include <iostream>

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