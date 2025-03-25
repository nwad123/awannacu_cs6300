#include "solvers.hpp"
#include "core.hpp"
#include "serial/serial.hpp"
#include "solver/concepts.hpp"
#include "basic_images.hpp"
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <filesystem>
#include <ranges>

using namespace std::views;

// ------ Test Images -----
const auto [flat_plane_storage, flat_plane] = detail::flat_plane_fn<10, 10>();
const auto [sloped_plane_storage, sloped_plane] = detail::sloped_plane_fn<10, 10>(1, 1);

// ------ Test Functions -----
template<Solver ...S>
auto print_names([[maybe_unused]] S... solvers)
{
    fmt::println("Added solvers:");
    (fmt::println(" - {}", S::name), ...);
}

// ------ Tests -----
TEST(Serial, FlatPlane) {
    Serial serial;
    auto output = serial.solve(flat_plane, {5, 5});
    auto map = Kokkos::mdspan(output.data(), flat_plane.extents());

    fmt::print("  ");
    for (const auto x : iota(0) | take(map.extent(0))) {
        fmt::print("{:<2}", x);
    }
    fmt::println("");

    for (const auto y : iota(0) | take(map.extent(1))) {
        fmt::print("{:<2}", y);
        for (const auto x : iota(0) | take(map.extent(0))) {
            fmt::print("{}", map(x, y) ? "X " : "  ");
        }
        fmt::println("");
    }
}
