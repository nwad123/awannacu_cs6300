#pragma once

#include "core.hpp"
#include <filesystem>
#include <numeric>
#include <algorithm>
#include <vector>

auto solve(const std::filesystem::path input_file, const std::filesystem::path output_file, const size_t width = 6000, const size_t height = 6000) -> void;

namespace detail {
    auto solve(mat_2d_i16 heights, mat_2d_i16 outputs) -> void;
    
    template<size_t Radius>
    auto circle_points() -> std::vector<std::pair<int64_t, int64_t>>;
    
    template<typename T>
    auto is_visible_from(const vec2<T> from, const vec2<T> to, const mat_2d_i16 heights, mat_2d_u8 seen, const int16_t vantage = 0) -> int16_t;

    constexpr size_t Radius = 100;
    constexpr size_t SeenDim = 2 * (Radius);
}

template<size_t Radius>
auto detail::circle_points() -> std::vector<std::pair<int64_t, int64_t>>
{
    static_assert(Radius > 0, "Radius must be positive.");

    // Calculate the estimated circumference.  Use a double for intermediate calculation.
    const double circumference = 2.0 * 3.14159 * Radius;

    // Estimate the number of points.  This is an approximation.
    const size_t estimated_num_points = static_cast<size_t>(std::ceil(circumference));

    // Reserve space in the vector.
    std::vector<std::pair<int64_t, int64_t>> points;
    points.reserve(estimated_num_points);

    // Set the intial values up to calculate cirle points
    int64_t x = 0;
    int64_t y = Radius;
    int64_t d = 3 - 2 * Radius;

    // Calculate values of the first octant (and then swap the values around for the rest)
    while (x <= y) {
        points.push_back({x, y});     // Octant 1
        points.push_back({y, x});     // Octant 2
        points.push_back({-x, y});    // Octant 4
        points.push_back({-y, x});    // Octant 3
        points.push_back({x, -y});    // Octant 8
        points.push_back({y, -x});    // Octant 7
        points.push_back({-x, -y});   // Octant 5
        points.push_back({-y, -x});   // Octant 6
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }

    // first sort the points according to the y-value
    std::sort(points.begin(), points.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    // then arrange them according to the x-value
    std::stable_sort(points.begin(), points.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    return points;
}

template<typename T>
auto detail::is_visible_from(const vec2<T> from, const vec2<T> to, const mat_2d_i16 heights, mat_2d_u8 seen, const int16_t vantage) -> int16_t
{
    const auto dx = std::abs(to.x - from.x);
    const auto dy = std::abs(to.y - from.y);
    const auto sx = from.x < to.x ? 1 : -1;
    const auto sy = from.y < to.y ? 1 : -1;

    auto err = dx - dy;

    auto x = from.x;
    auto y = from.y;

    // we only use a seen vector that is big enough to cover the circle we look at 
    // so we need a way to translate the (x,y) coordinates into coordinates of the 
    // seen vector
    const auto top_left_x = from.x - static_cast<int64_t>(detail::Radius);
    auto translate_to_seen_coordinates_x = [&](const auto x) -> int64_t {
        return x - top_left_x;
    };

    const auto top_left_y = from.y - static_cast<int64_t>(detail::Radius);
    auto translate_to_seen_coordinates_y = [&](const auto y) -> int64_t {
        return y - top_left_y;
    };

    // this is an approximation of the distance that each line takes. This is 
    // precalculated in order to facilitate simpler operations in the hot loop
    const auto step = [&]() {
        const auto taxi_cab_length = dx + dy;
        const auto step_as_double = static_cast<double>(detail::Radius) / static_cast<double>(taxi_cab_length);
        return step_as_double;
    }();

    // Bresenhams algorithm in 2d (starting at `from` and going to `to`)
    // This is the first call to the algorithm, and it only continues while the points along the current
    // path have been seen 
    while(1)
    {
        if (x == to.x && y == to.y) {
            break;
        }

        if (!seen(translate_to_seen_coordinates_x(x), translate_to_seen_coordinates_y(y))) {
            break;
        }
        
        const auto e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }

    // Bresenhams algorithm in 2d (starting at `from` and going to `to`)
    // This is the second call to the algorithm, where the seen squares are actually added up.
    int16_t seen_count = 0;
    double max_angle = std::numeric_limits<double>::lowest();
    const auto from_height = heights(from.x, from.y) + vantage;
    auto length = double{0};

    while(1)
    {
        if (x == to.x && y == to.y) {
            break;
        }
        
        const auto e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }

        const auto angle_approx = [&](){
            const auto height = heights(x, y);
            const auto z_ = height - from_height;

            length += step;

            return static_cast<double>(z_) / length;
        }();

        // if angle_approx is >= max_angle that means we can see this point so we 
        // will increment the seen count and mark this square as seen
        if (angle_approx >= max_angle) {
            max_angle = angle_approx;
            const auto x_ = translate_to_seen_coordinates_x(x);
            const auto y_ = translate_to_seen_coordinates_y(y);
            auto& seen_ = seen(x_, y_);

            if (!seen_) {
                seen_count++;
                seen_ = true;
            }
        }
    }

    return seen_count;
}