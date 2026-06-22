#include <cmath>

/// <summary>
/// A 2D vector class. Only for numeric types (int, float, double, etc.). Provides basic vector operations and geometric functions.
/// </summary>
template <typename T>
struct Vector2
{
	T x, y;

	// Constructors
	Vector2() : x(0), y(0) {}
	Vector2(T x, T y) : x(x), y(y) {}

	// Operator overloads
	Vector2 operator+(const Vector2& other) const {
		return Vector2(x + other.x, y + other.y);
	}
	Vector2 operator-(const Vector2& other) const {
		return Vector2(x - other.x, y - other.y);
	}
	Vector2 operator*(T scaler) const {
		return Vector2(x * scaler, y * scaler);
	}
	Vector2 operator/(T scaler) const {
		return Vector2(x / scaler, y / scaler);
	}


	//Geometric functions
	float lengthSq() const {
		return x * x + y * y;
	}
	float magnitude() const {
		return sqrt(lengthSq());
	}

	Vector2 normalized() const {
		float len = magnitude();
		if (len == 0) return Vector2(0, 0);
		return Vector2(x / len, y / len);
	}

	//static utility functions

	static float Dot(const Vector2& a, const Vector2& b) {
		return a.x * b.x + a.y * b.y;
	}

	static float Cross(const Vector2& a, const Vector2& b) {
		return a.x * b.y - a.y * b.x;
	}

	static float distance(const Vector2& a, const Vector2& b) {
		return (a - b).magnitude();
	}

	

};