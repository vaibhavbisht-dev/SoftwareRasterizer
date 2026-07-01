#pragma once
#include <cmath>
#include <chrono>
#include "Vector3.h"

class MathUtility
{
public:
	/// <summary>
	/// This function adds a rotation to a point around a given axis by a specified angle in degrees.
	/// </summary>
	/// <param name="point">The current Memory location stored</param>
	/// <param name="axis">The axis around which to rotate</param>
	/// <param name="angle">The angle in degrees by which to rotate</param>
	/// <returns>Update points value as Degrees</returns>
	static void AddRotation(Vector3<float>& point, Vector3<float> axis, float angle) {
		// 1. Calculate and update each axis directly component-by-component
		point.x = std::fmod(point.x + (axis.x * angle), 360.0f);
		if (point.x < 0.0f) point.x += 360.0f;

		point.y = std::fmod(point.y + (axis.y * angle), 360.0f);
		if (point.y < 0.0f) point.y += 360.0f;

		point.z = std::fmod(point.z + (axis.z * angle), 360.0f);
		if (point.z < 0.0f) point.z += 360.0f;
	}
};

class Time {
public:
	Time() {
		Reset();
	}

	void Tick() {
		auto currentFrameTime = std::chrono::steady_clock::now();

		// Calculate elapsed time between frames in seconds
		std::chrono::duration<float> elapsed = currentFrameTime - m_lastFrameTime;
		m_deltaTime = elapsed.count();

		// Update the last frame time to the current time
		m_lastFrameTime = currentFrameTime;
	}
	

	void Reset() {
		m_lastFrameTime = std::chrono::steady_clock::now();
		m_deltaTime = 0.0f;
	}

	float DeltaTime() const {
		return m_deltaTime;
	}

private:
	std::chrono::steady_clock::time_point m_lastFrameTime;
	float m_deltaTime = 0.0f;
};