#pragma once
#include <cmath> // Make sure this is included for std::sqrt

/// <summary>
/// A 4D vector class. Only for numeric types (int, float, double, etc.). Provides basic vector operations and geometric functions.
/// </summary>
template <typename T>
struct Vector4
{
    union {
        struct { T x, y, z, w; };
        T data[4];
    };

    // --- NEW: Default Constructor ---
    Vector4() : x(0), y(0), z(0), w(0) {}

    // --- NEW: Parameterized Constructor (Fixes Error C2440) ---
    Vector4(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {}

    T& operator[](int index) { return data[index]; }
    const T& operator[](int index) const { return data[index]; }

    // Operator overloads (These will now work perfectly with the new constructor)
    Vector4 operator+(const Vector4& other) const { return Vector4(x + other.x, y + other.y, z + other.z, w + other.w); }
    Vector4 operator-(const Vector4& other) const { return Vector4(x - other.x, y - other.y, z - other.z, w - other.w); }
    Vector4 operator*(T scaler) const { return Vector4(x * scaler, y * scaler, z * scaler, w * scaler); }
    Vector4 operator/(T scaler) const { return Vector4(x / scaler, y / scaler, z / scaler, w / scaler); }

    // Geometric functions
    float lengthSq() const { return (float)(x * x + y * y + z * z + w * w); }
    float magnitude() const { return std::sqrt(lengthSq()); }

    Vector4 normalized() const {
        float len = magnitude();
        if (len == 0) return Vector4(0, 0, 0, 0);
        // Casts added to prevent narrowing conversion warnings if T is int
        return Vector4((T)(x / len), (T)(y / len), (T)(z / len), (T)(w / len));
    }

    static float Dot(const Vector4& a, const Vector4& b) {
        return (float)(a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w);
    }
};