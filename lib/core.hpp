#pragma once

#include "bool.hpp"
#include "vec3.hpp"
#include "mdspan.hpp"
#include <filesystem>

// ----------- Data Structures -----------
using vec3_fp32 = vec3<float>;
using vec3_i16 = vec3<int16_t>;
using vec3_i32 = vec3<int32_t>;
using index3 = vec3<int64_t>;

using mat_2d_exts = Kokkos::dextents<size_t, 2>;
using mat_2d_i16 = Kokkos::mdspan<int16_t, mat_2d_exts>;

using data_type = mat_2d_i16;
// TODO: check if we can actually use std::vector<bool> or not
// Also, we probably just want to use `mdspan`?
using output_type = std::vector<bool>;


// ----------- Functions -----------
auto shared() -> void;

/// Reads the input file in the given format.
/// @param input_file The path to the input file 
/// @returns The data values from the input file as a std::vector
[[nodiscard]]
auto read_input(const std::filesystem::path input_file) -> std::vector<int16_t>;

/// Formats the input into a nice 2-dimensional format
///
/// If we had C++23 we could use std::mdspan, but alas we are as 
/// beggars in this world and must satisfy ourselves with C++20.
///
/// @param input_data A span over the input data. The input data must be 
///                   contiguous in memory and of uint16_t type.
/// @param width The width of the input image 
/// @param height The height of the input image 
/// @returns A simple two dimensional span over the data. Note that `input_data`
///          is still the **owner** of the data, this just returns a span 
///          over it.
[[nodiscard]]
auto format_input(std::span<int16_t> input_data, 
                  const size_t width, 
                  const size_t length) -> mat_2d_i16;
