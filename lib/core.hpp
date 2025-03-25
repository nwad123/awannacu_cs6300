#pragma once

#include "solver/concepts.hpp"
#include <filesystem>

auto shared() -> void;

/// Reads the input file in the given format.
/// @param input_file The path to the input file 
/// @returns The data values from the input file as a std::vector
[[nodiscard]]
auto read_input(const std::filesystem::path input_file) -> std::vector<uint16_t>;
