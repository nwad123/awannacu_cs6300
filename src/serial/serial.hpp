#pragma once

#include "core.hpp"
#include <filesystem>

auto solve(const std::filesystem::path input_file, const std::filesystem::path output_file, const size_t width = 6000, const size_t height = 6000) -> void;

namespace detail {
    auto solve(mat_2d_f32 heights, mat_2d_i16 outputs) -> void;
    template<typename T>
    auto is_visible_from(vec2<T> from, vec2<T> to, mat_2d_f32 heights) -> int16_t;
}

template<typename T>
auto detail::is_visible_from(vec2<T> from, vec2<T> to, mat_2d_f32 heights) -> int16_t
{
    // get the starting and ending points in vec3 form
    const auto src = vec3{static_cast<float>(from.x), static_cast<float>(from.y), heights(from.x, from.y)};
    const auto dst = vec3{static_cast<float>(to.x), static_cast<float>(to.y), heights(to.x, to.y)};

    // get the pointing vector from source to destination
    const auto dir = dst - src;

    // set the step size to 1, this means we'll get repeat comparisons but that is 
    // totally okay.
    // OPTIMIZATION: use another algorithm to optimize the step size
    const auto dir_norm = normalize(dir);

    auto start = src;
    while ((start - src).magnitude() < dir.magnitude())
    {
        // progress the ray forward
        start = start + dir_norm;

        // get the x and y coordinates
        const auto x = static_cast<int64_t>(std::round(start.x));
        const auto y = static_cast<int64_t>(std::round(start.y));

        // check if the point is higher than the current height
        if (heights(x, y) > start.z) {
            // return no success
            return 0;
        }
    }

    // return success
    return 1;
}