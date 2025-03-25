#include "solvers.hpp"
#include "core.hpp"
#include "solver/concepts.hpp"
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <filesystem>

template<Solver ...S>
auto print_names([[maybe_unused]] S... solvers)
{
    fmt::println("Added solvers:");
    (fmt::println(" - {}", S::name), ...);
}

TEST(Serial, OriginalTest) {
    std::filesystem::path("__temp__.raw");
}
