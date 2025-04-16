#pragma once

#include <cmath>

// DISCLAIMER: I gave my vec3 implementation to Gemini, and I told it to write
// this for me.

template<typename T>
struct vec2 {
    using value_type = T;

    T x{0};
    T y{0};

    constexpr vec2() = default;
    constexpr vec2(const T x, const T y) : x(x), y(y) {}
    constexpr vec2(const vec2<T>& other) : x(other.x), y(other.y) {}


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
            std::abs(initial.x),
            std::abs(initial.y)
        };
    }

    constexpr friend vec2 normalize(const vec2<T>& initial) {
        const auto magnitude = std::sqrt(initial.x * initial.x + initial.y * initial.y);
        if (magnitude == 0) {
            return {0, 0};
        }
        return {
            initial.x / magnitude,
            initial.y / magnitude
        };
    }

};