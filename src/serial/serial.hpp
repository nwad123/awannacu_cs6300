#pragma once

#include "core.hpp"
#include "solver/concepts.hpp"
#include <string_view>

class Serial
{
public:
    static constexpr std::string_view name = "Serial";
    auto solve(const data_type data, const index3 at) -> output_type;
};

static_assert(Solver<Serial>);
