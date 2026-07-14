#include "Renderer.h"
#include <thread>

// 1. Properly initialize the FrameBuffer reference in the constructor initializer list
Renderer::Renderer(int width, int height, FrameBuffer& framebuffer)
    : m_width(width), m_height(height), m_framebuffer(framebuffer),
    m_cameraPos(0.0f, 2.0f, 7.0f), m_rotationDegree(0.0f, 0.0f, 0.0f),
    m_window(nullptr), m_sdlRenderer(nullptr), m_sdlTexture(nullptr) {}

Renderer::~Renderer() {
    if (m_sdlTexture) SDL_DestroyTexture(m_sdlTexture);
    if (m_sdlRenderer) SDL_DestroyRenderer(m_sdlRenderer);
    if (m_window) SDL_DestroyWindow(m_window);
    SDL_Quit();
}

bool Renderer::Initialize() {
    if (!SDL_Init(SDL_INIT_VIDEO)) return false;

    m_window = SDL_CreateWindow("Software Rasterizer", m_width, m_height, 0);
    if (!m_window) return false;

    m_sdlRenderer = SDL_CreateRenderer(m_window, nullptr);
    if (!m_sdlRenderer) return false;

    m_sdlTexture = SDL_CreateTexture(m_sdlRenderer, SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING, m_width, m_height);
    if (!m_sdlTexture) return false;

    // Configure your framebuffer settings via reference
    m_framebuffer.SetUsingZBuffer(true);
    m_framebuffer.SetLightSource(Vector3<float>(0.0f, 5.0f, 8.0f));
    m_framebuffer.SetCameraPosition(m_cameraPos);
    m_framebuffer.SetProjectionMatrix(60.0f, (float)m_width / (float)m_height, 0.1f, 100.0f);

    return true;
}

// 2. New Setter Functions
void Renderer::SetCameraPosition(const Vector3<float>& position) {
    m_cameraPos = position;
    m_framebuffer.SetCameraPosition(m_cameraPos);
}

void Renderer::SetCameraRotation(const Vector3<float>& rotationDegree) {
    m_rotationDegree = rotationDegree;
}

void Renderer::RenderFrame(const std::vector<Vertex>& vertices,
    const std::vector<Triangle>& triangles,
     SoftwareTexture& texture,
    float deltaTime) {
    m_framebuffer.clearbuffer();

    // 3. Object-level rotation vs Camera rotation updates
    // Note: If you want m_rotationDegree to control the CAMERA rotation, 
    // update your SetViewMatrix calculation here. If it controls the OBJECT, keep this as-is:
    m_rotationDegree.y += 25.5f * deltaTime;

    m_framebuffer.CreateModelMatrix(Vector3<float>(0, 0, 0), m_rotationDegree, Vector3<float>(1, 1, 1));
    m_framebuffer.SetViewMatrix(m_cameraPos, Vector3<float>(0, 0, 0), Vector3<float>(0, 1, 0));
    m_framebuffer.ComputeMVPMatrix();

    // Vertex Stage
    m_transformedVertices.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++) {
        m_transformedVertices[i] = m_framebuffer.TransformVertex(vertices[i]);
    }
	// Rasterization Stage
	std::vector<std::thread> threads;
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;
	int rowsPerThread = m_height / numThreads;
    for (int i = 0; i < numThreads; i++) {
		int minRow = i * rowsPerThread;
        int maxRow = (i == numThreads -1)? m_height - 1 : (minRow + rowsPerThread - 1);
        threads.emplace_back(&FrameBuffer::RenderBand, &m_framebuffer, minRow, maxRow, std::cref(m_transformedVertices), std::cref(triangles), std::ref(texture) );
    }
    for (auto& t : threads) t.join();

    // Output Present Stage
    SDL_UpdateTexture(m_sdlTexture, nullptr, m_framebuffer.getBuffer().data(), m_width * sizeof(uint32_t));
    SDL_RenderTexture(m_sdlRenderer, m_sdlTexture, nullptr, nullptr);
    SDL_RenderPresent(m_sdlRenderer);
}
