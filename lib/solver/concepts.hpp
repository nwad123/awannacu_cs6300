#pragma once 

#include "core.hpp"
#include <string_view>

template<class S>
concept Solver = requires(S& s, const data_type data) {
    { S::name } -> std::convertible_to<const std::string_view>;
    { s.solve(data) } -> std::same_as<output_type>;
} || requires(S& s, const std::span<int16_t> data) {
    { S::name } -> std::convertible_to<const std::string_view>;
    { s.solve(data) } -> std::same_as<output_type>;
};
