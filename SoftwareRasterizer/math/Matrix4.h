#pragma once
#include <iostream>
#include "Vector3.h"
#include "Vector4.h"
#include <cassert>


struct Matrix4
{
    // Array indices mapped explicitly:
    // [0] m00  [4] m01  [8]  m02  [12] m03
    // [1] m10  [5] m11  [9]  m12  [13] m14
    // [2] m20  [6] m21  [10] m22  [14] m24
    // [3] m30  [7] m31  [11] m32  [15] m35
    float m[16];

    // Default Constructor: Initializes immediately to Identity Matrix
    Matrix4() {
        for (int i = 0; i < 16; ++i) m[i] = 0.0f;
        m[0] = 1.0f;
        m[5] = 1.0f;
        m[10] = 1.0f;
        m[15] = 1.0f;
    }

    // Element access via standard (row, column) syntax
    float& operator()(int row, int col) {
        assert(row >= 0 && row < 4 && col >= 0 && col < 4);
        return m[col * 4 + row];
    }

    const float& operator()(int row, int col) const {
        assert(row >= 0 && row < 4 && col >= 0 && col < 4);
        return m[col * 4 + row];
    }

    // Matrix Multiplication (Row x Column)
    Matrix4 operator*(const Matrix4& other) const {
        Matrix4 result;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += (*this)(row, k) * other(k, col);
                }
                result(row, col) = sum;
            }
        }
        return result;
    }

	

    // Matrix x Column-Vector Multiplication
    Vector4<float> operator*(const Vector4<float>& vec) const {
        Vector4<float> result;
        for (int row = 0; row < 4; ++row) {
            float sum = 0.0f;
            for (int col = 0; col < 4; ++col) {
                sum += (*this)(row, col) * vec[col];
            }
            result[row] = sum;
        }
        return result;
    }

    static Matrix4 Identity() {
        return Matrix4();
    }

    static Matrix4 Translation(float x, float y, float z) {
        Matrix4 result; // Inherits identity base values
        result(0, 3) = x;
        result(1, 3) = y;
        result(2, 3) = z;
        return result;
    }

    static Matrix4 Scaling(float x, float y, float z) {
        Matrix4 result; // Inherits identity base values
        result(0, 0) = x;
        result(1, 1) = y;
        result(2, 2) = z;
        return result;
    }

    static Matrix4 RotationX(float degrees) {
        Matrix4 result = Matrix4::Identity();
        float pi = 3.1415926535f;
        float radians = degrees * (pi / 180.0f);

        float c = std::cos(radians);
        float s = std::sin(radians);

        // 0-indexed rows/columns for X-rotation (modifying Y and Z axes)
        result(1, 1) = c;
        result(1, 2) = -s;
        result(2, 1) = s;
        result(2, 2) = c;

        return result;
    }

    static Matrix4 RotationY(float degrees) {
        Matrix4 result = Matrix4::Identity();
        float pi = 3.1415926535f;
        float radians = degrees * (pi / 180.0f);

        float c = std::cos(radians);
        float s = std::sin(radians);

        // Fixed standard signs for Right-Handed system
        result(0, 0) = c;
        result(0, 2) = s;   // Positive s
        result(2, 0) = -s;  // Negative s
        result(2, 2) = c;
        return result;
    }

    static Matrix4 RotationZ(float degrees) {
        // 1. Initialize as an identity matrix first
        Matrix4 result = Matrix4::Identity();

        // 2. Convert degrees to radians
        float pi = 3.1415926535f;
        float radians = degrees * (pi / 180.0f);

        float c = std::cos(radians);
        float s = std::sin(radians);
        result(0, 0) = c;
        result(0, 1) = -s;
        result(1, 0) = s;
        result(1, 1) = c;
        return result;
    }

    // Standard Right-Handed Perspective Projection Matrix (OpenGL-style)
    static Matrix4 Perspective(float fovDegree, float aspect, float nearPlane, float farPlane) {
        Matrix4 result;
        // Zero out structural diagonal properties first
        for (int i = 0; i < 16; ++i) result.m[i] = 0.0f;

        float fovRadians = fovDegree * (3.1415926535f / 180.0f);
        float tanHalfFov = std::tan(fovRadians / 2.0f);
        result(0, 0) = 1.0f / (aspect * tanHalfFov);
        result(1, 1) = 1.0f / tanHalfFov;
        result(2, 2) = -(farPlane + nearPlane) / (farPlane - nearPlane);
        result(2, 3) = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        result(3, 2) = -1.0f;
        return result;
    }

    // Standard Right-Handed LookAt Camera Matrix
    static Matrix4 LookAt(const Vector3<float>& eye, const Vector3<float>& center, const Vector3<float>& up) {
        Vector3<float> f = (center - eye).normalized();
        Vector3<float> s = Vector3<float>::Cross(f, up).normalized();
        Vector3<float> u = Vector3<float>::Cross(s, f);

        Matrix4 result;
        // Construct inverse camera alignment orientation into rows
        result(0, 0) = s.x;  result(0, 1) = s.y;  result(0, 2) = s.z;
        result(1, 0) = u.x;  result(1, 1) = u.y;  result(1, 2) = u.z;
        result(2, 0) = -f.x; result(2, 1) = -f.y; result(2, 2) = -f.z;

        // Apply camera inverted target dot products directly to the translation column
        result(0, 3) = -Vector3<float>::Dot(s, eye);
        result(1, 3) = -Vector3<float>::Dot(u, eye);
        result(2, 3) = Vector3<float>::Dot(f, eye);
        return result;
    }

    void Print() const {
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                std::cout << (*this)(row, col) << "\t";
            }
            std::cout << "\n";
        }
    }
};