#pragma once

#include "solver/concepts.hpp"
#include "vec3.hpp"
#include <concepts>
#include <filesystem>

// ----------- Data Structures -----------
using vec3_fp32 = vec3<float>;
using vec3_i32 = vec3<int32_t>;

// ----------- Functions -----------
auto shared() -> void;

/// Reads the input file in the given format.
/// @param input_file The path to the input file 
/// @returns The data values from the input file as a std::vector
[[nodiscard]]
auto read_input(const std::filesystem::path input_file) -> std::vector<uint16_t>;
