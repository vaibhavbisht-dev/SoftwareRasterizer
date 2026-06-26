#pragma once

/// <summary>
/// A 3D vector class. Only for numeric types (int, float, double, etc.). Provides basic vector operations and geometric functions.
/// </summary>
template <typename T>
struct Vector3
{
	T x, y, z;

	// Constructors
	Vector3() : x(0), y(0), z(0) {}
	Vector3(T x, T y, T z) : x(x), y(y), z(z) {}

	// Operator overloads
	Vector3 operator+(const Vector3& other) const {
		return Vector3(x + other.x, y + other.y, z + other.z);
	}
	Vector3 operator-(const Vector3& other) const {
		return Vector3(x - other.x, y - other.y, z - other.z);
	}
	Vector3 operator*(T scaler) const {
		return Vector3(x * scaler, y * scaler, z * scaler);
	}
	Vector3 operator/(T scaler) const {
		return Vector3(x / scaler, y / scaler, z / scaler);
	}


	//Geometric functions
	float lengthSq() const {
		return x * x + y * y + z * z;
	}
	float magnitude() const {
		return sqrt(lengthSq());
	}

	Vector3 normalized() const {
		float len = magnitude();
		if (len == 0) return Vector3(0, 0, 0);
		return Vector3(x / len, y / len, z / len);
	}

	//static utility functions

	static float Dot(const Vector3& a, const Vector3& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	static Vector3 Cross(const Vector3& a, const Vector3& b) {
		return Vector3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}

	static float distance(const Vector3& a, const Vector3& b) {
		return (a - b).magnitude();
	}



};