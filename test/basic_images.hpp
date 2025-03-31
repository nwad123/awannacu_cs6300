#include "mdspan.hpp"
#include "core.hpp"
#include <array>
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

static inline std::array<int16_t, 100> double_sin_storage = {0, 4, -3, -1, 4, -2, -2, 4, -1, -3,
4, 9, 0, 3, 9, 1, 1, 9, 3, 0,
-3, 0, -7, -5, 1, -6, -6, 1, -5, -7,
-1, 3, -5, -2, 3, -4, -4, 3, -2, -5,
4, 9, 1, 3, 9, 2, 2, 9, 3, 1,
-2, 1, -6, -4, 2, -5, -5, 2, -4, -6,
-2, 1, -6, -4, 2, -5, -5, 2, -4, -6,
4, 9, 1, 3, 9, 2, 2, 9, 3, 1,
-1, 3, -5, -2, 3, -4, -4, 3, -2, -5,
-3, 0, -7, -5, 1, -6, -6, 1, -5, -7};

static inline auto double_sin = Kokkos::mdspan(double_sin_storage.data(), 10, 10);
