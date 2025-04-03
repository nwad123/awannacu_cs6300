#include "solvers.hpp"
#include "core.hpp"
#include "fmt/base.h"
#include "serial/serial.hpp"
#include "solver/concepts.hpp"
#include "basic_images.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/color.h>
#include <filesystem>
#include <ranges>
#include <span>

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


auto print_map(auto data, auto map, index2 point) -> void 
{
    const auto [min_height, height_range] = [&](){
        const auto heights = std::span(data.data_handle(), data.extent(0) * data.extent(1));
        const auto [min, max] = std::ranges::minmax(heights);

        return std::pair{min, max - min};
    }();

    auto color_map = [&](const auto z){
        const double z_ = static_cast<double>(z);
        const double min_ = static_cast<double>(min_height);
        const double range_ = [&](){
            if (height_range != 0)
                return static_cast<double>(height_range);
            else 
                return 1.0;
        }();

        const auto range = std::lerp(0.0, 1.0, (z_ - min_) / range_);

        auto as_u8 = [](auto n) { return static_cast<uint8_t>(n); };

        const auto r_ = as_u8(std::lerp(0.0, 255.0, range));
        const auto g_ = as_u8(std::lerp(255.0, 0.0, range));
        const auto b_ = as_u8(std::lerp(0.0, 0.0, range));

        return fmt::rgb(r_, g_, b_);
    };

    fmt::print("   ");
    for (const auto x : iota(0) | take(map.extent(0))) {
        fmt::print("{:^3}", x);
    }
    fmt::println("");

    for (const auto y : iota(0) | take(map.extent(1))) {
        fmt::print("{:^3}", y);
        for (const auto x : iota(0) | take(map.extent(0))) {
            if (x == point.x && y == point.y)
                fmt::print(fmt::emphasis::bold | fmt::emphasis::italic | fg(fmt::color::black) | bg(color_map(data(x, y))), "{}", " O ");
            else 
                fmt::print(fg(fmt::color::black) | bg(color_map(data(x, y))), "{}", map(x, y) ? " + " : "   ");
        }
        fmt::println("");
    }
}

// ------ Tests -----
TEST(Serial, FlatPlane1) {
    index2 point = {5, 5};
    Serial serial;
    auto output = serial.solve(flat_plane, point);
    auto map = Kokkos::mdspan(output.data(), flat_plane.extents());
    print_map(flat_plane, map, point);
}

TEST(Serial, SlopedPlane1) {
    index2 point = {5, 5};
    Serial serial;
    auto output = serial.solve(sloped_plane, point);
    auto map = Kokkos::mdspan(output.data(), sloped_plane.extents());
    print_map(sloped_plane, map, point);
}

TEST(Serial, VerticalHill1_top) {
    index2 point = {2, 2};
    Serial serial;
    auto output = serial.solve(vertical_hill, point);
    auto map = Kokkos::mdspan(output.data(), vertical_hill.extents());
    print_map(vertical_hill, map, point);
}

TEST(Serial, VerticalHill2_base) {
    index2 point = {2, 0};
    Serial serial;
    auto output = serial.solve(vertical_hill, point);
    auto map = Kokkos::mdspan(output.data(), vertical_hill.extents());
    print_map(vertical_hill, map, point);
}

TEST(Serial, Cone1_Center) {
    index2 point = {2, 2};
    Serial serial;
    auto output = serial.solve(cone, point);
    auto map = Kokkos::mdspan(output.data(), cone.extents());
    print_map(cone, map, point);
}

TEST(Serial, Cone2_Base) {
    index2 point = {4, 4};
    Serial serial;
    auto output = serial.solve(cone, point);
    auto map = Kokkos::mdspan(output.data(), cone.extents());
    print_map(cone, map, point);
}

TEST(Serial, Cone3_Side) {
    index2 point = {3, 3};
    Serial serial;
    auto output = serial.solve(cone, point);
    auto map = Kokkos::mdspan(output.data(), cone.extents());
    print_map(cone, map, point);
}

TEST(Serial, SteepCone1_Side) {
    index2 point = {1, 1};
    Serial serial;
    auto output = serial.solve(steep_cone, point);
    auto map = Kokkos::mdspan(output.data(), steep_cone.extents());
    print_map(steep_cone, map, point);
}

TEST(Serial, NegCone1_Center) {
    index2 point = {2, 2};
    Serial serial;
    auto output = serial.solve(neg_cone, point);
    auto map = Kokkos::mdspan(output.data(), neg_cone.extents());
    print_map(neg_cone, map, point);
}

TEST(Serial, NegCone2_Side) {
    index2 point = {1, 1};
    Serial serial;
    auto output = serial.solve(neg_cone, point);
    auto map = Kokkos::mdspan(output.data(), neg_cone.extents());
    print_map(neg_cone, map, point);
}

TEST(Serial, DoubleSin1_Center) {
    std::ranges::transform(double_sin_storage, double_sin_storage.begin(), [](auto x) { return x + 7; });
    index2 point = {4, 4};
    Serial serial;
    auto output = serial.solve(double_sin, point);
    auto map = Kokkos::mdspan(output.data(), double_sin.extents());
    print_map(double_sin, map, point);
}

TEST(Serial, DoubleSin1_Depth) {
    std::ranges::transform(double_sin_storage, double_sin_storage.begin(), [](auto x) { return x + 7; });
    index2 point = {2, 2};
    Serial serial;
    auto output = serial.solve(double_sin, point);
    auto map = Kokkos::mdspan(output.data(), double_sin.extents());
    print_map(double_sin, map, point);
}
