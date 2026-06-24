#include "FrameBuffer.h"


FrameBuffer::FrameBuffer(int width, int height)
	: m_width(width), m_height(height), m_buffer(width* height, 0xFF000000)
{}

FrameBuffer::~FrameBuffer()
{}


void FrameBuffer::clearbuffer()
{
	std::fill(m_buffer.begin(), m_buffer.end(), 0xFF000000);
}

void FrameBuffer::setPixel(int x, int y, uint32_t color) {
	if (x >= 0 && x < m_width && y >= 0 && y < m_height)
	{
		m_buffer[y * m_width + x] = color;
	}
}

void FrameBuffer::DrawLine(int x0, int y0, int x1, int y1, uint32_t color)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if(x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	// as this is same for every line, we can calculate it once
	int dx = x1 - x0;
	int dy = y1 - y0;

	// We use error term to decide when to step in y direction. The error term is the distance from the ideal line. When it exceeds a threshold (in this case, dx), we step in y direction and reset the error.
	int derror = std::abs(dy) * 2;
	int error = 0;


	int y = y0;

	for (int x = x0; x <= x1; x++) {
		if (steep) {
			setPixel(y, x, color); // if transposed, de−transpose 
		}
		else {
			setPixel(x, y, color);
		}

		// Update the error term and step in y direction if necessary
		error += derror;
		if (error > dx) {
			y += (y1 > y0 ? 1 : -1);
			error -= dx *2;
		}
	}
}

void FrameBuffer::DrawTriangle(Vector2<int> t0, Vector2<int> t1, Vector2<int> t2, uint32_t color)
{
	
	int min_X = std::min({ t0.x, t1.x, t2.x });
	int max_X = std::max({ t0.x, t1.x, t2.x });
	int min_Y = std::min({ t0.y, t1.y, t2.y });
	int max_Y = std::max({ t0.y, t1.y, t2.y });

	min_X = std::max(0, min_X);
	max_X = std::min(m_width - 1, max_X);
	min_Y = std::max(0, min_Y);
	max_Y = std::min(m_height - 1, max_Y);

	for (int y = min_Y; y <= max_Y; y++) {
		for (int x = min_X; x <= max_X; x++) {
			// Check if the point (x, y) is inside the triangle
			// Implementation for point-in-triangle check would go here
			if (isPointInTriangle(Vector2<int>(x, y), t0, t1, t2)) {
				setPixel(x, y, color);
			}
		}
	}
}


bool FrameBuffer::isPointInTriangle(Vector2<int> p, Vector2<int> a, Vector2<int> b, Vector2<int> c) {
	
	int e0 = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
	int e1 = (c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x);
	int e2 = (a.x - c.x) * (p.y - c.y) - (a.y - c.y) * (p.x - c.x);

	return (e0 < 0 && e1 < 0 && e2 < 0);

}

