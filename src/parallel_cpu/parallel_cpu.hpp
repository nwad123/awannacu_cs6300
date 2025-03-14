#pragma once

#include "core.hpp"
#include <string_view>

class ParallelCPU
{
public:
    static constexpr std::string_view name = "ParallelCPU";
    auto solve(const data_type data) -> output_type;
};

static_assert(Solver<ParallelCPU>);