#include "serial.hpp"
#include "core.hpp"
#include <fstream>
#include <algorithm>
#include <fmt/core.h>

auto solve(const std::filesystem::path input_file, const std::filesystem::path output_file, const size_t width, const size_t height) -> void {
    // Check if the input file exists.
    if (!std::filesystem::exists(input_file)) {
        fmt::println("Error: Input file {} does not exist.", input_file.string());
        return;
    }

    // Check if the input file has the correct extension.
    if (input_file.extension() != ".raw") {
        fmt::println("Error: Input file {} is not a .raw file.", input_file.string());
        return;
    }

    // Check if the output file has the correct extension.
    if (output_file.extension() != ".raw") {
        fmt::println("Error: Output file {} is not a .raw file.", output_file.string());
        return;
    }

    const auto parent_path = output_file.parent_path();
    if (!parent_path.empty() && !std::filesystem::exists(parent_path)) {
        // Create the output directory if it doesn't exist.
        std::filesystem::create_directories(parent_path);
    }

    // Read the input file.
    auto heights = read_input(input_file);

    // Validate the input file
    if (heights.size() != height * width) {
        fmt::println("Error: Input file {} has an incorrect size. Expected {} values, but got {}.", input_file.string(), height * width, heights.size());
        return;
    }

    // Reuse the ints vector as the output vector (sneaky sneaky, I know)
    auto outputs = std::vector<int16_t>(heights.size(), 0);

    // Wrap the heights and outputs in multi-dimensional spans 
    auto h = to_span(tcb::span(heights.data(), heights.size()), width, height);
    auto o = to_span(tcb::span(outputs.data(), outputs.size()), width, height);

    // Call the solving algorithm
    detail::solve(h, o);

     // Write results to the output file
     std::ofstream output(output_file, std::ios::binary);
     if (!output.is_open()) {
         fmt::println("Failed to open output file: {}", output_file.string());
         return;
     }
 
     output.write(reinterpret_cast<const char*>(outputs.data()), static_cast<long>(outputs.size() * sizeof(int16_t)));
     output.close();
}

auto detail::solve(mat_2d_i16 heights, mat_2d_i16 outputs) -> void {
    if (heights.extents() != outputs.extents()) {
        fmt::println("Spans passed into the solver are not equivalently sized!");
        return;
    }

    // precompute the circular offsets
    const auto pixel_offsets = circle_points<Radius>();

    // Set up the static storage for the "seen" variables
    auto seen_storage = std::vector<uint8_t>(SeenDim * SeenDim, false);
    auto seen = Kokkos::mdspan(seen_storage.data(), SeenDim, SeenDim);

    // Checks if point is in array
    auto is_valid_point = [&](const auto x, const auto y) {
        return x >= 0 && x < heights.extent(0) && y >= 0 && y < heights.extent(1);
    };

    // Check visibility for each point
    for (int64_t y = 0; y < heights.extent(1); y++) {
        for (int64_t x = 0; x < heights.extent(0); x++) {
            // ensure the outputs starts at 0
            outputs(x, y) = 0;

            std::fill(seen_storage.begin(), seen_storage.end(), false);

            // calculate how many points can be seen from (x,y)
            for (const auto& [x_offset, y_offset] : pixel_offsets) {
                const auto x_ = x + x_offset;
                const auto y_ = y + y_offset;

                if (is_valid_point(x_, y_)) {
                    outputs(x, y) += detail::is_visible_from(vec2{x, y}, vec2{x_, y_}, heights, seen, 6);
                }
            }
        }
    }
}
