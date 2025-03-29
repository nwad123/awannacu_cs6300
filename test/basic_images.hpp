#include "mdspan.hpp"
#include "core.hpp"
#include <array>
#include <utility>
#include <ranges>

namespace detail {

// Returns an owning, stack based array that underneath is set up to a WxH mdspan
// flat_plane_fn<5, 4>() -> array
//
// this image represents a purely flat plane. every location should be visible 
// from every other location.
template<size_t W, size_t H>
[[nodiscard]]
constexpr auto flat_plane_fn() -> std::array<int16_t, W*H> {
    auto arr = std::array<int16_t, W*H>{};
    arr.fill(0);

    return arr;
}

// Returns an owning, stack based array that underneath is set up to a WxH mdspan
// sloped_plane_fn<5, 4>(1, 1) -> array
//
// The plane is sloped on the x-axis according to `x` and sloped on the y-axis 
// according to `y`. There is no offset.
//
// Every location should be visible from every other location.
template<size_t W, size_t H>
[[nodiscard]]
constexpr auto sloped_plane_fn(const int16_t x, const int16_t y) -> std::array<int16_t, W*H> {
    using namespace std::views;

    auto arr = std::array<int16_t, W*H>{};
    auto span = mat_2d_i16(arr.data(), W, H);

    constexpr auto zero = static_cast<int16_t>(0);

    for (const auto x_index : iota(zero) | take(W)) {
        for (const auto y_index : iota(zero) | take(H)) {
            span(x_index, y_index) = x * x_index + y * y_index;
        }
    }

    return arr;
}
}

static inline std::array<int16_t, 25> vertical_hill_storage = 
    { 0, 1, 2, 1, 0,
      0, 1, 2, 1, 0,
      0, 1, 2, 1, 0,
      0, 1, 2, 1, 0,
      0, 1, 2, 1, 0};

static inline auto vertical_hill = Kokkos::mdspan(vertical_hill_storage.data(), 5, 5);

static inline std::array<int16_t, 25> cone_storage = 
    { 0, 0, 0, 0, 0,
      0, 1, 1, 1, 0,
      0, 1, 2, 1, 0,
      0, 1, 1, 1, 0,
      0, 0, 0, 0, 0};

static inline auto cone = Kokkos::mdspan(cone_storage.data(), 5, 5);

static inline std::array<int16_t, 100> double_sin_storage = {0, 90, -75, -27, 98, -54, -53, 99, -28, -75,
90, 181, 15, 62, 189, 36, 37, 189, 62, 15,
-75, 15, -151, -103, 23, -130, -129, 23, -104, -150,
-27, 62, -103, -55, 70, -82, -81, 71, -56, -103,
98, 189, 23, 70, 197, 44, 45, 197, 70, 23,
-54, 36, -130, -82, 44, -108, -108, 44, -83, -129,
-53, 37, -129, -81, 45, -108, -107, 45, -82, -128,
99, 189, 23, 71, 197, 44, 45, 198, 70, 23,
-28, 62, -104, -56, 70, -83, -82, 70, -57, -103,
-75, 15, -150, -103, 23, -129, -128, 23, -103, -150};

static inline auto double_sin = Kokkos::mdspan(double_sin_storage.data(), 10, 10);
