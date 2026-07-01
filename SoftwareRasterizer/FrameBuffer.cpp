#include "FrameBuffer.h"


FrameBuffer::FrameBuffer(int width, int height)
	: m_width(width), m_height(height), m_buffer(width* height, 0xFF000000), m_zbuffer(width * height, std::numeric_limits<float>::max())
{}

FrameBuffer::~FrameBuffer()
{}


void FrameBuffer::clearbuffer()
{
	if (IsUsingZBuffer) {
		std::fill(m_zbuffer.begin(), m_zbuffer.end(), std::numeric_limits<float>::max());
	}
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

void FrameBuffer::DrawTriangle(Vector3<float> t0, Vector3<float> t1, Vector3<float> t2, uint32_t color)
{
	
	int min_X = (int)std::floor(std::min({ t0.x, t1.x, t2.x }));
	int max_X = (int)std::ceil(std::max({ t0.x, t1.x, t2.x }));
	int min_Y = (int)std::floor(std::min({ t0.y, t1.y, t2.y }));
	int max_Y = (int)std::ceil(std::max({ t0.y, t1.y, t2.y }));

	min_X = std::max(0, min_X);
	max_X = std::min(m_width - 1, max_X);
	min_Y = std::max(0, min_Y);
	max_Y = std::min(m_height - 1, max_Y);

	for (int y = min_Y; y <= max_Y; y++) {
		for (int x = min_X; x <= max_X; x++) {
			// Check if the point (x, y) is inside the triangle
			// Implementation for point-in-triangle check would go here
			BarycentricResults bary = computeBarycentricCoordinates(Vector3<float>(x, y, 0), t0, t1, t2);

			if (bary.isInside) {
				if (IsUsingZBuffer) {
					// Perform Z-buffering check
					int bufferIndex = y * m_width + x;
					if (bary.depth < m_zbuffer[bufferIndex]) {
						m_zbuffer[bufferIndex] = bary.depth;
						setPixel(x, y, color);
					}
				} else {
					setPixel(x, y, color);
				}
				
			}
		}
	}
}



BarycentricResults FrameBuffer::computeBarycentricCoordinates(Vector3<float> p, Vector3<float> a, Vector3<float> b, Vector3<float> c) {
	BarycentricResults bary;
	float e0 = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
	float e1 = (c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x);
	float e2 = (a.x - c.x) * (p.y - c.y) - (a.y - c.y) * (p.x - c.x);
	
	if (e0 > 0 || e1 > 0 || e2 > 0) {
		bary.isInside = false;
		bary.w0 = -1.0f;
		bary.w1 = -1.0f;
		bary.w2 = -1.0f;
		bary.depth = -1;
		return bary;
	}

	float total_area = e0 + e1 + e2;

	// Safety Check: If the triangle is flat or a straight line, skip computation
	if (std::abs(total_area) < 1e-6f) {
		bary.isInside = false;
		bary.w0 = -1.0f;
		bary.w1 = -1.0f;
		bary.w2 = -1.0f;
		bary.depth = -1;
		return bary;
	}

	float w0 = static_cast<float>(e0) / total_area;
	float w1 = static_cast<float>(e1) / total_area;
	float w2 = static_cast<float>(e2) / total_area;

	bary.w0 = w0;
	bary.w1 = w1;
	bary.w2 = w2;

	bary.depth = w0 * a.z + w1 * b.z + w2 * c.z;

	return bary;
}

void FrameBuffer::CreateModelMatrix(Vector3<float> translation, Vector3<float> rotation, Vector3<float> scale) {
	Matrix4 translationMatrix = Matrix4::Translation(translation.x, translation.y, translation.z);
	Matrix4 rotationXMatrix = Matrix4::RotationX(rotation.x);
	Matrix4 rotationYMatrix = Matrix4::RotationY(rotation.y);
	Matrix4 rotationZMatrix = Matrix4::RotationZ(rotation.z);
	Matrix4 scaleMatrix = Matrix4::Scaling(scale.x, scale.y, scale.z);

	// Combine all 3 rotations into one total rotation matrix (Z * Y * X)
	Matrix4 rotationMatrix = rotationZMatrix * rotationYMatrix * rotationXMatrix;

	m_modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}


void FrameBuffer::SetViewMatrix(Vector3<float> eye, Vector3<float> center, Vector3<float> up) {
	m_viewMatrix = Matrix4::LookAt(eye, center, up);
}

void FrameBuffer::SetProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) {
	m_projectionMatrix = Matrix4::Perspective(fov, aspectRatio, nearPlane, farPlane);
}

Vector3<float> FrameBuffer::TransformVertex(Vector3<float> vertex) {
	Vector4<float> vertex4(vertex.x, vertex.y, vertex.z, 1.0f);
	

	vertex4 = m_mvpMatrix * vertex4;

	Vector3<float> NDC = Vector3<float>(vertex4.x / vertex4.w, vertex4.y / vertex4.w, vertex4.z / vertex4.w);
	
	Vector3<float> screenVector = Vector3<float>(
		(NDC.x + 1.0f) * 0.5f * m_width,
		(1.0f - NDC.y) * 0.5f * m_height,
		NDC.z
	);

	return screenVector;
	
}

// using this function to compute the MVP matrix after setting model, view, and projection matrices
void FrameBuffer::ComputeMVPMatrix() {
	m_mvpMatrix = m_projectionMatrix * m_viewMatrix * m_modelMatrix;
}
