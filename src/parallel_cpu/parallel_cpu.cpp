#include "parallel_cpu.hpp"
#include "core.hpp"
#include <fmt/core.h>
#include <algorithm>

#ifdef _OPENMP 
#include <omp.h>
#endif

// Parallel CPU implementation
// This implementation will use the openMP library to parallelize the computation
// of each point in the file. The file will then set a bit in the shared memory output
// file. 

// When every point has been checked, the file will be written back.



/// Returns 'true' when the points are visible to each other
[[nodiscard]] 
auto check_visibility(const data_type data, index3 p1, index3 p2) -> bool;

auto ParallelCPU::solve(const data_type data, const index2 at) -> output_type
{
    // Add a vantage to the viewpoint, later we can inject this into 
    // the function.
    constexpr auto VANTAGE = 2LL;

    // get the height and width of the input data 
    const auto width = data.extent(0);
    const auto height = data.extent(1);

    // allocate some space for the output 
    output_type output_vec(width * height, false);

    // make a little wrapper for the output with a span 
    auto output = Kokkos::mdspan(output_vec.data(), data.extents());

    // get the starting location
    const auto from = vec3{at.x, at.y, data(at.x, at.y) + VANTAGE};

    // check visibility of each point to each other point 
    #pragma omp parallel for schedule(dynamic) collapse(2)
    for (int64_t x  = 0; x < static_cast<int64_t>(width); x++) {
        for (int64_t y  = 0; y < static_cast<int64_t>(width); y++) {
            const auto z = data(x, y);
            output(x, y) = check_visibility(data, from, {x, y, z});
        }
    }

    return output_vec; 
}

auto check_visibility(const data_type data, index3 p1, index3 p2) -> bool
{
    const auto delta = (p2 - p1);
    const auto delta_abs = abs(delta);

    auto set_dir = [](const auto l, const auto r) {
        if (l > r) return 1;
        else if (l == r) return 0;
        else return -1;
    };

    const auto xs = set_dir(p2.x, p1.x);
    const auto ys = set_dir(p2.y, p1.y);
    const auto zs = set_dir(p2.z, p1.z);

    const auto max = std::max({delta_abs.x, delta_abs.y, delta_abs.z});

    bool valid{true};

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
            
            if (p1.z  < data(p1.x, p1.y)) { valid = false; }
        }
    }
    else if (max == delta_abs.y) {
        auto p_1 = 2 * delta_abs.x - delta_abs.y;
        auto p_2 = 2 * delta_abs.z - delta_abs.y;

        while (p1.y != p2.y) {
            p1.y += ys;
            if (p_1 >= 0) {
                p1.x += xs;
                p_1 -= 2 * delta_abs.y;
            }
            if (p_2 >= 0) {
                p1.z += zs;
                p_2 -= 2 * delta_abs.y;
            }
            p_1 += 2 * delta_abs.x;
            p_2 += 2 * delta_abs.z;
            
            if (p1.z  < data(p1.x, p1.y)) { valid = false; }
        }
    }
    else {
        auto p_1 = 2 * delta_abs.y - delta_abs.z;
        auto p_2 = 2 * delta_abs.x - delta_abs.z;
    
        while (p1.z != p2.z) {
            p1.z += zs;
            if (p_1 >= 0) {
                p1.y += ys;
                p_1 -= 2 * delta_abs.z;
            }
            if (p_2 >= 0) {
                p1.x += xs;
                p_2 -= 2 * delta_abs.z;
            }
            p_1 += 2 * delta_abs.y;
            p_2 += 2 * delta_abs.x;
            
            if (p1.z  < data(p1.x, p1.y)) { valid = false; }
        }
    }

    return valid;
}

