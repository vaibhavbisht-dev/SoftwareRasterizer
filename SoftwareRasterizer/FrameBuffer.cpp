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
	if (t0.y == t1.y && t0.y == t2.y) return;
	// Sort the vertices by y-coordinate ascending (t0.y <= t1.y <= t2.y)
	if (t0.y > t1.y) std::swap(t0, t1);
	if (t1.y > t2.y) std::swap(t1, t2);
	if (t0.y > t1.y) std::swap(t0, t1);

	int total_height = t2.y - t0.y;
	if (total_height == 0) return; // Prevent division by zero

	// We will iterate through each horizontal line of the triangle, calculating the intersection points with the triangle edges and filling in the pixels between those points.
	for (int i = 0; i < total_height; i++)
	{
		// Determine if we are in the upper or lower part of the triangle
		bool second_half = i > t1.y - t0.y || t1.y == t0.y;
		// Calculate the height of the current segment (upper or lower)
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
		// Calculate the interpolation factors for the current y level
		float alpha = (float)i / total_height;
		float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here
		// Interpolate the x-coordinates of the triangle edges
		int ax =				t0.x + (t2.x - t0.x) * alpha;
		int bx = second_half ? t1.x + (t2.x - t1.x) * beta : t0.x + (t1.x - t0.x) * beta;
		// Ensure ax is the leftmost point and bx is the rightmost point
		if (ax > bx) std::swap(ax, bx);
		// We are drawing a horizontal line from ax to bx at the current y level (t0.y + i)
		for (int x = ax; x <= bx; x++)
		{
			setPixel(x, t0.y + i, color);
		}
	}
}

