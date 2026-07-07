#pragma once

#include <vector>
#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "math/Matrix4.h"
#include <cstdint>
#include <algorithm>
#include "helper/SoftwareTexture.h"


struct BarycentricResults
{
	float w0, w1, w2;
	bool isInside = true;
	float depth;
};
struct Vertex
{
	Vector3<float> position;
	Vector2<float> uv;
	Vector3<float> normal;

};
struct TransformedVertex
{
	Vector3<float> position;
	Vector2<float> uv;
	Vector3<float> worldPos;
	Vector3<float> normal;
	float invW;
};

class FrameBuffer
{
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();


	void clearbuffer();
	void setPixel(int x, int y, uint32_t color);

	void DrawLine(int x0, int y0, int x1, int y1, uint32_t color);
	void DrawTriangle(TransformedVertex v0, TransformedVertex v1, TransformedVertex v2, SoftwareTexture& texture);


	std::vector<uint32_t>& getBuffer() { return m_buffer; }

	void SetUsingZBuffer(bool useZBuffer) { IsUsingZBuffer = useZBuffer; }

	void CreateModelMatrix(Vector3<float> translation, Vector3<float> rotation, Vector3<float> scale);
	void SetViewMatrix(Vector3<float> eye, Vector3<float> center, Vector3<float> up);
	void SetProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane);
	TransformedVertex TransformVertex(Vertex vertex);
	void SetLightSource(Vector3<float> lightSource) { m_lightSource = lightSource; }
	void SetCameraPosition(Vector3<float> cameraPosition) { m_camPOS = cameraPosition; }
	float ComputeLightIntensity(Vector3<float> worldPos, Vector3<float> normal);
	void ComputeMVPMatrix();

	void SetIntensities(float ambient, float diffuse, float specular) {
		m_ambientIntensity = ambient;
		m_diffuseIntensity = diffuse;
		m_specularIntensity = specular;
	}
	float GetAmbientIntensity() const { return m_ambientIntensity; }
	float GetDiffuseIntensity() const { return m_diffuseIntensity; }
	float GetSpecularIntensity() const { return m_specularIntensity; }
	


private:
	int m_width;
	int m_height;
	std::vector<uint32_t> m_buffer;
	std::vector<float> m_zbuffer;
	BarycentricResults computeBarycentricCoordinates(Vector3<float> p, Vector3<float> a, Vector3<float> b, Vector3<float> c);

	Vector3<float> m_lightSource;
	Vector3<float> m_camPOS;
	float m_ambientIntensity = 0.1f;
	float m_diffuseIntensity = 0.8f;
	float m_specularIntensity = 0.5f;


	bool IsUsingZBuffer = false; // Flag to indicate whether to use Z-buffering or not

	Matrix4 m_modelMatrix = Matrix4::Identity();
	Matrix4 m_viewMatrix = Matrix4::Identity();
	Matrix4 m_projectionMatrix = Matrix4::Identity();

	Matrix4 m_mvpMatrix = Matrix4::Identity(); // Model-View-Projection matrix
	
};



