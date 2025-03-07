#pragma once

#include "core.hpp"
#include <string_view>

class ParallelGPU
{
public:
    static constexpr std::string_view name = "ParallelGPU";
    auto solve(const data_type data) -> output_type;
};

static_assert(Solver<ParallelGPU>);