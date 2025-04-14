#include "distributed_cpu.hpp"
#include "parallel_cpu.hpp"
#include "core.hpp"
#include <fmt/core.h>
#include <algorithm>

[[nodiscard]] 
auto check_visibility(const data_type data, index3 p1, index3 p2) -> bool;

auto DistributedCPU::solve(const data_type data, const index2 at) -> output_type
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
    const auto from = vec3{at.x, at.y, static_cast<decltype(at.y)>(data(at.x, at.y) + VANTAGE)};

    // check visibility of each point to each other point 
    
    //The main implmentation should use a scatter. No reduction is needed. 

    for (int64_t x  = 0; x < static_cast<int64_t>(width); x++) {
        for (int64_t y  = 0; y < static_cast<int64_t>(height); y++) {
            const auto z = data(x, y);
            output(x, y) = check_visibility(data, from, {x, y, z});
        }
    }

    return output_vec; 
}