#pragma once

#include "bool.hpp"
#include "vec3.hpp"
#include "vec2.hpp"
#include "mdspan.hpp"
#include <filesystem>
#include <vector>
#include <span.hpp>
#include <fmt/core.h>

// ----------- Data Structures -----------
using vec3_i16 = vec3<int16_t>;
using index3 = vec3<int64_t>;

using vec2_i16 = vec2<int16_t>;
using index2 = vec2<int64_t>;

using mat_2d_exts = Kokkos::dextents<size_t, 2>;
using mat_2d_u8 = Kokkos::mdspan<uint8_t, mat_2d_exts>;
using mat_2d_i16 = Kokkos::mdspan<int16_t, mat_2d_exts>;
using mat_2d_f32 = Kokkos::mdspan<float, mat_2d_exts>;



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
/// beggars in this world and must satisfy ourselves with C++20. Luckily
/// Kokkos made a C++20 compatible version of std::mdspan that we'll 
/// use.
///
/// @param input_data A span over the input data. The input data must be 
///                   contiguous in memory and of uint16_t type.
/// @param width The width of the input image 
/// @param height The height of the input image 
/// @returns A simple two dimensional span over the data. Note that `input_data`
///          is still the **owner** of the data, this just returns a span 
///          over it.
template<typename T>
[[nodiscard]]
auto to_span(tcb::span<T> input_data, 
                  const size_t width, 
                  const size_t length) -> Kokkos::mdspan<T, mat_2d_exts>
{
    if (width * length != input_data.size()) {
        fmt::println("Input size mismatch!");
        return {};
    }

    return Kokkos::mdspan(input_data.data(), width, length);
}
                
