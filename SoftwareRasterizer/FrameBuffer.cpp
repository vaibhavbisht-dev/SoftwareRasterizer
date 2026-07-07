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

void FrameBuffer::DrawTriangle(TransformedVertex v0, TransformedVertex v1, TransformedVertex v2, SoftwareTexture& texture)
{
	
	int min_X = (int)std::floor(std::min({ v0.position.x, v1.position.x, v2.position.x }));
	int max_X = (int)std::ceil(std::max({ v0.position.x, v1.position.x, v2.position.x }));
	int min_Y = (int)std::floor(std::min({ v0.position.y, v1.position.y, v2.position.y }));
	int max_Y = (int)std::ceil(std::max({ v0.position.y, v1.position.y, v2.position.y }));

	min_X = std::max(0, min_X);
	max_X = std::min(m_width - 1, max_X);
	min_Y = std::max(0, min_Y);
	max_Y = std::min(m_height - 1, max_Y);

	uint8_t r, g, b;

	for (int y = min_Y; y <= max_Y; y++) {
		for (int x = min_X; x <= max_X; x++) {
			float signedArea = (v1.position.x - v0.position.x) * (v2.position.y - v0.position.y)
				- (v1.position.y - v0.position.y) * (v2.position.x - v0.position.x);
			if (signedArea >= 0.0f) return;

			BarycentricResults bary = computeBarycentricCoordinates(Vector3<float>(x, y, 0), v0.position, v1.position, v2.position);
			


			if (bary.isInside) {
				uint8_t currentBaseR = 0;
				uint8_t currentBaseG = 0;
				uint8_t currentBaseB = 0;

				float interpolatedInvW = bary.w0 * v0.invW + bary.w1 * v1.invW + bary.w2 * v2.invW;
				Vector2<float> trueUV = (bary.w0 * v0.uv * v0.invW + bary.w1 * v1.uv * v1.invW + bary.w2 * v2.uv * v2.invW) / interpolatedInvW;

				Vector3<float> interpolated_Normal = (bary.w0 * v0.normal * v0.invW + bary.w1 * v1.normal * v1.invW + bary.w2 * v2.normal * v2.invW) / interpolatedInvW;
				Vector3<float> interpolated_WorldPos = (bary.w0 * v0.worldPos * v0.invW + bary.w1 * v1.worldPos * v1.invW + bary.w2 * v2.worldPos * v2.invW) / interpolatedInvW;

				float intensity = ComputeLightIntensity(interpolated_WorldPos, interpolated_Normal);

				if (IsUsingZBuffer) {
					// Perform Z-buffering check
					int bufferIndex = y * m_width + x;
					if (bary.depth < m_zbuffer[bufferIndex]) {
						m_zbuffer[bufferIndex] = bary.depth;
						texture.Sample(trueUV.x, trueUV.y, currentBaseR, currentBaseG, currentBaseB);

						uint8_t phongR = (uint8_t)std::clamp(currentBaseR * intensity, 0.0f, 255.0f);
						uint8_t phongG = (uint8_t)std::clamp(currentBaseG * intensity, 0.0f, 255.0f);
						uint8_t phongB = (uint8_t)std::clamp(currentBaseB * intensity, 0.0f, 255.0f);

						setPixel(x, y, (0xFF << 24) | (phongR << 16) | (phongG << 8) | phongB);
					}
				} else {
					texture.Sample(trueUV.x, trueUV.y, currentBaseR, currentBaseG, currentBaseB);
					uint8_t phongR = (uint8_t)std::clamp(currentBaseR * intensity, 0.0f, 255.0f);
					uint8_t phongG = (uint8_t)std::clamp(currentBaseG * intensity, 0.0f, 255.0f);
					uint8_t phongB = (uint8_t)std::clamp(currentBaseB * intensity, 0.0f, 255.0f);

					setPixel(x, y, (0xFF << 24) | (phongR << 16) | (phongG << 8) | phongB);
				}

			}
		}
	}
}

float FrameBuffer::ComputeLightIntensity(Vector3<float> worldPos, Vector3<float> normal) {
	
	// 1. Normalize input vectors
	Vector3<float> N = normal.normalized();

	// 2. Compute light direction vector (L)
	Vector3<float> L = (m_lightSource - worldPos).normalized();

	// 3. Calculate Diffuse component (Lambert's Cosine Law)
	float diffuse_factor = std::max(0.0f, Vector3<float>::Dot(N, L));
	float diffuse = m_diffuseIntensity * diffuse_factor;

	// 4. Calculate Specular component using True Blinn-Phong
	Vector3<float> viewDir = (m_camPOS - worldPos).normalized(); // Use the set camera position
	Vector3<float> H = (L + viewDir).normalized(); // Halfway vector

	float specular_factor = std::pow(std::max(0.0f, Vector3<float>::Dot(N, H)), 32.0f); // Shininess = 32
	float specular = m_specularIntensity * specular_factor;

	// 5. Combine components
	return m_ambientIntensity + diffuse + specular;

	
}



BarycentricResults FrameBuffer::computeBarycentricCoordinates(Vector3<float> p, Vector3<float> a, Vector3<float> b, Vector3<float> c) {
	BarycentricResults bary;
	float e0 = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x); // edge(a,b) -> weight for c
	float e1 = (c.x - b.x) * (p.y - b.y) - (c.y - b.y) * (p.x - b.x); // edge(b,c) -> weight for a
	float e2 = (a.x - c.x) * (p.y - c.y) - (a.y - c.y) * (p.x - c.x); // edge(c,a) -> weight for b

	if (e0 > 0 || e1 > 0 || e2 > 0) {
		bary.isInside = false;
		bary.w0 = bary.w1 = bary.w2 = -1.0f;
		bary.depth = -1;
		return bary;
	}

	float total_area = e0 + e1 + e2;
	if (std::abs(total_area) < 1e-6f) {
		bary.isInside = false;
		bary.w0 = bary.w1 = bary.w2 = -1.0f;
		bary.depth = -1;
		return bary;
	}

	// Correct assignment: e1 -> a (w0), e2 -> b (w1), e0 -> c (w2)
	float w0 = e1 / total_area;
	float w1 = e2 / total_area;
	float w2 = e0 / total_area;

	bary.w0 = w0;
	bary.w1 = w1;
	bary.w2 = w2;
	bary.isInside = true;

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

TransformedVertex FrameBuffer::TransformVertex(Vertex vertex) {
	TransformedVertex transformedVertex;
	Vector4<float> vertex4(vertex.position.x, vertex.position.y, vertex.position.z, 1.0f);
	
	Vector4<float> tempWorldPos = m_modelMatrix * vertex4;
	transformedVertex.worldPos.x = tempWorldPos.x;
	transformedVertex.worldPos.y = tempWorldPos.y;
	transformedVertex.worldPos.z = tempWorldPos.z;

	vertex4 = m_mvpMatrix * vertex4;

	Vector3<float> NDC = Vector3<float>(vertex4.x / vertex4.w, vertex4.y / vertex4.w, vertex4.z / vertex4.w);
	
	Vector3<float> screenVector = Vector3<float>(
		(NDC.x + 1.0f) * 0.5f * m_width,
		(1.0f - NDC.y) * 0.5f * m_height,
		NDC.z
	);

	transformedVertex.position = screenVector;
	transformedVertex.uv = vertex.uv; // Use the UV coordinates from the input vertex
	transformedVertex.invW = 1.0f / vertex4.w;

	// Transform the normal using the model matrix (ignoring translation)
	Vector4<float> normal4(vertex.normal.x, vertex.normal.y, vertex.normal.z, 0.0f);
	Vector4<float> tempNormal = m_modelMatrix * normal4;
	transformedVertex.normal = Vector3<float>(tempNormal.x, tempNormal.y, tempNormal.z).normalized();

	return transformedVertex;
	
}

// using this function to compute the MVP matrix after setting model, view, and projection matrices
void FrameBuffer::ComputeMVPMatrix() {
	m_mvpMatrix = m_projectionMatrix * m_viewMatrix * m_modelMatrix;
}
