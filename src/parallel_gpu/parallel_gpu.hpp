#pragma once

#include "solver/concepts.hpp"
#include <string_view>

class ParallelGPU
{
public:
    static constexpr std::string_view name = "ParallelGPU";
    auto solve(const data_type data, const index3 at) -> output_type;
};

static_assert(Solver<ParallelGPU>);
