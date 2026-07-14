#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"
#include <vector>
#include <cstdint>

struct Vertex
{
	Vector3<float> position;
	Vector2<float> uv;
	Vector3<float> normal;
};

struct Triangle {
	int v0, v1, v2;
	uint32_t color;
};

struct ObjectData {
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
};
