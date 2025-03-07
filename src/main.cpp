#include "solvers.hpp"
#include "core.hpp"
#include <fmt/core.h>

template<Solver ...S>
auto print_names([[maybe_unused]] S... solvers)
{
    fmt::println("Added solvers:");
    (fmt::println(" - {}", S::name), ...);
}

auto main() -> int
{
    shared();
    print_names(Serial{}, ParallelCPU{}, ParallelGPU{}, DistributedCPU{}, DistributedGPU{});
}