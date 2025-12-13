#pragma once;

#include <cmath>

struct Vector3 {
    float x, y, z;

    constexpr Vector3() noexcept : x(0), y(0), z(0) { }
    constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) { }

    constexpr Vector3& operator+=(Vector3 v) noexcept { x += v.x; y += v.y; z += v.z; return *this; }
    constexpr Vector3& operator-=(Vector3 v) noexcept { x -= v.x; y -= v.y; z -= v.z; return *this; }
    constexpr Vector3& operator*=(float s)   noexcept { x *= s; y *= s; z *= s; return *this; }
    constexpr Vector3& operator/=(float s)   noexcept { x /= s; y /= s; z /= s; return *this; }
};

constexpr Vector3 operator+(Vector3 a, Vector3 b) noexcept { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
constexpr Vector3 operator-(Vector3 a, Vector3 b) noexcept { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
constexpr Vector3 operator*(Vector3 v, float s) noexcept { return {v.x * s, v.y * s, v.z * s}; }
constexpr Vector3 operator*(float s, Vector3 v) noexcept { return v * s; }
constexpr Vector3 operator/(Vector3 v, float s) noexcept { return {v.x / s, v.y / s, v.z / s}; }

constexpr bool operator==(Vector3 a, Vector3 b) noexcept { return a.x == b.x && a.y == b.y && a.z == b.z; }
constexpr bool operator!=(Vector3 a, Vector3 b) noexcept { return !(a == b); }

constexpr float dot(Vector3 a, Vector3 b) noexcept { return a.x*b.x + a.y*b.y + a.z*b.z; }
constexpr Vector3 cross(Vector3 a, Vector3 b) noexcept { return { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x }; }
inline float lengthSq(Vector3 v) noexcept { return dot(v, v); }
inline float length(Vector3 v) noexcept { return std::sqrt(lengthSq(v)); }

inline Vector3 normalized(Vector3 v) noexcept {
    float len = length(v);
    return (len > 0.0f) ? v / len : Vector3{};
}

inline void normalize(Vector3& v) noexcept {
    float len = length(v);
    if (len > 0.0f)
        v /= len;
}

constexpr Vector3 lerp(Vector3 a, Vector3 b, float t) noexcept { return a + (b - a) * t; }

inline float distance(Vector3 a, Vector3 b) noexcept { return length(a - b); }