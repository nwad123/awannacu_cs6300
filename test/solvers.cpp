#include "solvers.hpp"
#include "core.hpp"
#include "serial/serial.hpp"
#include "solver/concepts.hpp"
#include "basic_images.hpp"
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/color.h>
#include <filesystem>
#include <ranges>

using namespace std::views;

// ------ Test Images -----
constexpr auto W = 10;
constexpr auto H = 10;

auto flat_plane_storage = detail::flat_plane_fn<W, H>();
auto flat_plane = Kokkos::mdspan(flat_plane_storage.data(), W, H); 

auto sloped_plane_storage = detail::sloped_plane_fn<W, H>(1, 1);
auto sloped_plane = Kokkos::mdspan(sloped_plane_storage.data(), W, H); 

// ------ Test Functions -----
template<Solver ...S>
auto print_names([[maybe_unused]] S... solvers)
{
    fmt::println("Added solvers:");
    (fmt::println(" - {}", S::name), ...);
}


auto print_map(auto map, index2 point) -> void 
{
    fmt::print("  ");
    for (const auto x : iota(0) | take(map.extent(0))) {
        fmt::print("{:<2}", x);
    }
    fmt::println("");

    for (const auto y : iota(0) | take(map.extent(1))) {
        fmt::print("{:<2}", y);
        for (const auto x : iota(0) | take(map.extent(0))) {
            if (x == point.x && y == point.y)
                fmt::print(fg(fmt::color::red), "{}", "X ");
            else 
                fmt::print("{}", map(x, y) ? "X " : "  ");
        }
        fmt::println("");
    }
}

// ------ Tests -----
TEST(Serial, FlatPlane) {
    index2 point = {5, 5};
    Serial serial;
    auto output = serial.solve(flat_plane, point);
    auto map = Kokkos::mdspan(output.data(), flat_plane.extents());
    print_map(map, point);
}

TEST(Serial, SlopedPlane) {
    index2 point = {5, 5};
    Serial serial;
    auto output = serial.solve(sloped_plane, point);
    auto map = Kokkos::mdspan(output.data(), sloped_plane.extents());
    print_map(map, point);
}
