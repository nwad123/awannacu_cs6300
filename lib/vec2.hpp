#pragma once

#include <concepts>
#include <cmath>
#include "vec3.hpp"

// DISCLAIMER: I gave my vec3 implementation to Gemini, and I told it to write
// this for me.

template<typename T>
requires std::integral<T> || std::floating_point<T>
struct vec2 {
    using value_type = T;

    T x{0};
    T y{0};

    constexpr vec2 operator+(const vec2& other) const {
        return {x + other.x, y + other.y};
    }

    constexpr vec2 operator-(const vec2& other) const {
        return {x - other.x, y - other.y};
    }

    constexpr vec2 operator*(const vec2& other) const {
        return {x * other.x, y * other.y};
    }

    constexpr vec2 operator/(const vec2& other) const {
        return {x / other.x, y / other.y};
    }

    constexpr vec2 operator+(const T& scalar) const {
        return {x + scalar, y + scalar};
    }

    constexpr vec2 operator-(const T& scalar) const {
        return {x - scalar, y - scalar};
    }

    constexpr vec2 operator*(const T& scalar) const {
        return {x * scalar, y * scalar};
    }

    constexpr vec2 operator/(const T& scalar) const {
        return {x / scalar, y / scalar};
    }

    constexpr friend vec2 operator+(const T& scalar, const vec2& v) {
        return v + scalar;
    }

    constexpr friend vec2 operator-(const T& scalar, const vec2& v) {
        return {scalar - v.x, scalar - v.y};
    }

    constexpr friend vec2 operator*(const T& scalar, const vec2& v) {
        return v * scalar;
    }

    constexpr friend vec2 operator/(const T& scalar, const vec2& v) {
        return {scalar / v.x, scalar / v.y};
    }

    constexpr friend vec2 abs(const vec2<T>& initial) {
        return {
            .x = std::abs(initial.x),
            .y = std::abs(initial.y)
        };
    }
    
    constexpr vec3<T> to_vec3(const T z) const {
        return { 
            .x = x, 
            .y = y,
            .z = z
        };
    }

};
