#pragma once

#include "solver/concepts.hpp"
#include <string_view>

class Serial
{
public:
    static constexpr std::string_view name = "Serial";
    auto solve(const data_type data) -> output_type;
};

static_assert(Solver<Serial>);
