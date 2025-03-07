#include "solvers.hpp"
#include <fmt/core.h>
#include "parallel_cpu/parallel_cpu.hpp"
template<Solver ...S>
auto print_names(S... solvers)
{
    (fmt::println("{}", S::name), ...);
}

auto main() -> int
{
    fmt::println("Hello main!");
    print_names(Serial{});
}