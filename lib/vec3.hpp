#include <concepts>

template<typename T>
requires std::integral<T> || std::floating_point<T>
struct vec3 {
    T x{0};
    T y{0};
    T z{0};

    constexpr vec3 operator+(const vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    constexpr vec3 operator-(const vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    constexpr vec3 operator*(const vec3& other) const {
        return {x * other.x, y * other.y, z * other.z};
    }

    constexpr vec3 operator/(const vec3& other) const {
        return {x / other.x, y / other.y, z / other.z};
    }

    constexpr vec3 operator+(const T& scalar) const {
        return {x + scalar, y + scalar, z + scalar};
    }

    constexpr vec3 operator-(const T& scalar) const {
        return {x - scalar, y - scalar, z - scalar};
    }

    constexpr vec3 operator*(const T& scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    constexpr vec3 operator/(const T& scalar) const {
        return {x / scalar, y / scalar, z / scalar};
    }

    constexpr friend vec3 operator+(const T& scalar, const vec3& v) {
        return v + scalar;
    }

    constexpr friend vec3 operator-(const T& scalar, const vec3& v) {
        return {scalar-v.x, scalar-v.y, scalar-v.z};
    }

    constexpr friend vec3 operator*(const T& scalar, const vec3& v) {
        return v * scalar;
    }

    constexpr friend vec3 operator/(const T& scalar, const vec3& v) {
        return {scalar/v.x, scalar/v.y, scalar/v.z};
    }
};
