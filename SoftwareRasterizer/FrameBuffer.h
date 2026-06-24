#pragma once

#include "iostream"
#include <vector>
#include "math/Vector2.h"
#include <cstdint>
#include <algorithm>


class FrameBuffer
{
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();

	void clearbuffer();
	void setPixel(int x, int y, uint32_t color);

	void DrawLine(int x0, int y0, int x1, int y1, uint32_t color);
	void DrawTriangle(Vector2<int> t0, Vector2<int> t1, Vector2<int> t2, uint32_t color);


	std::vector<uint32_t>& getBuffer() { return m_buffer; }



private:
	int m_width;
	int m_height;
	std::vector<uint32_t> m_buffer;

	bool isPointInTriangle(Vector2<int> p, Vector2<int> a, Vector2<int> b, Vector2<int> c);
};