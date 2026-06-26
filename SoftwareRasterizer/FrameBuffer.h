#pragma once

#include <vector>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include <cstdint>
#include <algorithm>


struct BarycentricResults
{
	float w0, w1, w2;
	bool isInside = true;
	float depth;
};

class FrameBuffer
{
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();

	void clearbuffer();
	void setPixel(int x, int y, uint32_t color);

	void DrawLine(int x0, int y0, int x1, int y1, uint32_t color);
	void DrawTriangle(Vector3<float> t0, Vector3<float> t1, Vector3<float> t2, uint32_t color);


	std::vector<uint32_t>& getBuffer() { return m_buffer; }

	void SetUsingZBuffer(bool useZBuffer) { IsUsingZBuffer = useZBuffer; }



private:
	int m_width;
	int m_height;
	std::vector<uint32_t> m_buffer;
	std::vector<float> m_zbuffer;
	BarycentricResults computeBarycentricCoordinates(Vector3<float> p, Vector3<float> a, Vector3<float> b, Vector3<float> c);

	bool IsUsingZBuffer = false; // Flag to indicate whether to use Z-buffering or not

	
};



