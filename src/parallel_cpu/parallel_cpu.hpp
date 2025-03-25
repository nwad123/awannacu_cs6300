#pragma once

#include "solver/concepts.hpp"
#include <string_view>

class ParallelCPU
{
public:
    static constexpr std::string_view name = "ParallelCPU";
    auto solve(const data_type data, const index3 at) -> output_type;
};

static_assert(Solver<ParallelCPU>);
