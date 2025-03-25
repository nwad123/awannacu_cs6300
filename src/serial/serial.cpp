#include "serial.hpp"
#include "core.hpp"

using point3_index = vec3<int64_t>;

/// Returns 'true' when the points are visible to each other
[[nodiscard]] 
auto check_visibility(const data_type data, const point3_index p1, const point3_index p2) -> bool;

auto Serial::solve(const data_type data, const index3 at) -> output_type
{
    // get the height and width of the input data 
    const auto width = data.extent(0);
    const auto height = data.extent(0);

    // allocate some space for the output 
    constexpr uint8_t TRUE = 1;
    constexpr uint8_t FALSE = 0;
    std::vector<uint8_t> output_vec(width * height, FALSE);

    // make a little wrapper for the output with a span 
    auto output = Kokkos::mdspan(output_vec.data(), data.extents());

    return {};
}

auto check_visibility(const data_type data, const point3_index p1, const point3_index p2) -> bool
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

    }
    else if (max == delta_abs.y) {

    }
    else {
    
    }

    return false;
}
