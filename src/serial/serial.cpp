#include "serial.hpp"
#include "core.hpp"
#include <fstream>
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
    const auto height_ints = read_input(input_file);

    // Validate the input file
    if (height_ints.size() != height * width) {
        fmt::println("Error: Input file {} has an incorrect size. Expected {} values, but got {}.", input_file.string(), height * width, height_ints.size());
        return;
    }

    // Convert the input file into floats
    auto heights = std::vector<float>(height_ints.size());
    std::transform(height_ints.begin(), height_ints.end(), heights.begin(), [](const auto i) { return static_cast<float>(i); });

    // Reuse the ints vector as the output vector (sneaky sneaky, I know)
    auto outputs = std::move(height_ints);

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

auto detail::solve(mat_2d_f32 heights, mat_2d_i16 outputs) -> void {
    if (heights.extents() != outputs.extents()) {
        fmt::println("Spans passed into the solver are not equivalently sized!");
        return;
    }

    // precompute the circular offsets
    const auto pixel_offsets = [&]() -> std::vector<std::pair<int, int>> {
        constexpr auto RADIUS = double{100};
        constexpr auto EXPECTED_AREA = double{RADIUS * RADIUS * 3.14159};

        // set up the offset area with the expected number of points being the area
        auto pixel_offsets = std::vector<std::pair<int, int>>();
        pixel_offsets.reserve(static_cast<size_t>(EXPECTED_AREA));

        // generate all pixel offsets that are within the circle
        for (int x = -static_cast<int>(RADIUS); x < RADIUS; x++) {
            for (int y = -static_cast<int>(RADIUS); y < RADIUS; y++) {
                // if the point is not in the circle OR at 0,0 skip it
                if (x * x + y * y <= RADIUS * RADIUS) { continue; }
                if (x == 0 && y == 0) { continue; }

                // insert the points inside of the circle
                pixel_offsets.emplace_back(x, y);
            }
        }

        return pixel_offsets;
    }();

    // Checks if point is in array
    auto is_valid_point = [&](const auto x, const auto y) {
        return x >= 0 && x < heights.extent(0) && y >= 0 && y < heights.extent(1);
    };

    // Check visibility for each point
    for (int64_t y = 0; y < heights.extent(1); y++) {
        for (int64_t x = 0; x < heights.extent(0); x++) {
            for (const auto& [x_offset, y_offset] : pixel_offsets) {
                // check if the offset point is inside the circle
                const auto x_ = x + x_offset;
                const auto y_ = y + y_offset;

                if (is_valid_point(x_, y_)) {
                    heights(x, y) += detail::is_visible_from(vec2{x, y}, vec2{x_, y_}, heights);
                }
            }
        }
    }
}
