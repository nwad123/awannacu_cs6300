#pragma once 

#include <span>
#include <string_view>
#include <vector>

using data_type = std::span<uint16_t>;
// TODO: check if we can actually use std::vector<bool> or not
using output_type = std::vector<bool>;

template<class S>
concept Solver = requires(S& s, const data_type data) {
    { S::name } -> std::convertible_to<const std::string_view>;
    { s.solve(data) } -> std::same_as<output_type>;
};