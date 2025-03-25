#pragma once 

#include "core.hpp"
#include <concepts>
#include <string_view>

template<class S>
concept Solver = 
requires(S& s, const data_type data, const index3 point) {
    { S::name } -> std::convertible_to<const std::string_view>;
    { s.solve(data, point) } -> std::same_as<output_type>;
};
