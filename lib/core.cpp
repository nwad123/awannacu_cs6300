#include "core.hpp"
#include <fstream>
#include <iostream>

auto shared() -> void
{
    fmt::println("I'm a shared library function!");
}

auto read_input(const std::filesystem::path input_file) -> std::vector<int16_t>
{
    // set up an empty vector to store the data 
    std::vector<int16_t> input_data;
    
    // check that the file is valid
    if (input_file.extension() != ".raw") {
        fmt::println("Can't open file with extension '{}'. Must have extension '.raw'", 
            input_file.extension().string());
        return input_data;
    }

    // open the file for binary reading
    std::ifstream input(input_file, std::ios::binary);
    if (!input.is_open()) {
        fmt::println("Failed to open input file: {}", input_file.string());
        return input_data;
    }

    // Get file size
    const auto file_size = [&]() -> size_t {
        // move cursor to end
        input.seekg(0, std::ios::end);
        // get length in bytes
        std::streampos size = input.tellg();
        // move cursor back to beginning
        input.seekg(0, std::ios::beg);

        // if the size if positive, return the size, otherwise return 0
        if (size > 0) {
            return static_cast<size_t>(size);
        } else {
            return 0;
        }
    }();

    // If the file size is not a multiple of uint16_t's then we have a problem 
    // we also have a problem if the file is empty
    if (file_size % sizeof(int16_t) != 0 || file_size == 0) {
        fmt::println("Input file {} opened, but has an invalid size of {} bytes!", input_file.string(), file_size);
        return input_data;
    }

    // pre-allocate the data in the input vector
    input_data.resize(file_size / sizeof(uint16_t));

    // read the file data into the vector directly
    input.read(reinterpret_cast<char*>(input_data.data()), static_cast<long>(file_size));

    // close the file
    input.close();

    // return our data
    return input_data;
}