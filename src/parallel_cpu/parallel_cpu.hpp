#pragma once

#include "solver/concepts.hpp"
#include <string_view>
#include <omp.h> 

#include <core.hpp>

class ParallelCPU
{
public:
    static constexpr std::string_view name = "ParallelCPU";
    auto solve(const data_type data, const index2 at) -> output_type;
};

static_assert(Solver<ParallelCPU>);

