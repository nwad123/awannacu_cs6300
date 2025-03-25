#pragma once

#include "solver/concepts.hpp"
#include <string_view>

class DistributedGPU
{
public:
    static constexpr std::string_view name = "DistributedGPU";
    auto solve(const data_type data, const index3 at) -> output_type;
};

static_assert(Solver<DistributedGPU>);
