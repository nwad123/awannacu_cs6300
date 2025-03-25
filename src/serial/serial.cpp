#include "serial.hpp"
#include "core.hpp"
#include <fmt/core.h>

/// Returns 'true' when the points are visible to each other
[[nodiscard]] 
auto check_visibility(const data_type data, index3 p1, index3 p2) -> bool;

auto Serial::solve(const data_type data, const index3 at) -> output_type
{
    // get the height and width of the input data 
    const auto width = data.extent(0);
    const auto height = data.extent(1);

    // allocate some space for the output 
    constexpr uint8_t TRUE = 1;
    constexpr uint8_t FALSE = 0;
    std::vector<uint8_t> output_vec(width * height, FALSE);

    // make a little wrapper for the output with a span 
    auto output = Kokkos::mdspan(output_vec.data(), data.extents());

    auto _ = check_visibility(data, {0, 0, 0}, {10, 0, 0});

    return {};
}

auto check_visibility(const data_type data, index3 p1, index3 p2) -> bool
{
    const auto delta = (p2 - p1);
    const auto delta_abs = abs(delta);

    auto set_dir = [](const auto l, const auto r) {
        if (l > r) return 1;
        else return -1;
    };

    const auto xs = set_dir(p2.x, p1.x);
    const auto ys = set_dir(p2.y, p1.y);
    const auto zs = set_dir(p2.z, p1.z);

    const auto max = std::max({delta_abs.x, delta_abs.y, delta_abs.z});

    if (max == delta_abs.x) {
        auto p_1 = 2 * delta_abs.y - delta_abs.x;
        auto p_2 = 2 * delta_abs.z - delta_abs.x;
        while (p1.x != p2.x) {
      p1.x += xs;
      if (p_1 >= 0) {
        p1.y += ys;
        p_1 -= 2 * delta_abs.x;
      }
      if (p_2 >= 0) {
        p1.z += zs;
        p_2 -= 2 * delta_abs.x;
      }
      p_1 += 2 * delta_abs.y;
      p_2 += 2 * delta_abs.z;
            fmt::println("({}, {}, {})", p1.x, p1.y, p1.z);
    }
    }
    else if (max == delta_abs.y) {
        const auto p1 = 2 * delta_abs.y - delta_abs.y;
        const auto p2 = 2 * delta_abs.x - delta_abs.y;

    }
    else {
        const auto p1 = 2 * delta_abs.y - delta_abs.z;
        const auto p2 = 2 * delta_abs.z - delta_abs.z;
    
    }

    return false;
}
