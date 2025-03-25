#include "solvers.hpp"
#include "core.hpp"
#include "serial/serial.hpp"
#include "solver/concepts.hpp"
#include "basic_images.hpp"
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <filesystem>

// ------ Test Images -----
const auto [flat_plane_storage, flat_plane] = detail::flat_plane_fn<100, 100>();
const auto [sloped_plane_storage, sloped_plane] = detail::sloped_plane_fn<100, 100>(1, 1);

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
}
