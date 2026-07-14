#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "../FrameBuffer.h"
#include "../helper/SoftwareTexture.h"
#include "../math/Vector3.h"
#include "../helper/Parser.h"
#include "../DataStructures.h"
#include "../Thread/ThreadPool.h"

class Renderer {
public:
    Renderer(int width, int height, FrameBuffer& framebuffer);
    ~Renderer();

    bool Initialize();
    void RenderFrame(const std::vector<Vertex>& vertices,
        const std::vector<Triangle>& triangles,
        SoftwareTexture& texture,
        float deltaTime);
	void SetCameraPosition(const Vector3<float>& position);
	void SetCameraRotation(const Vector3<float>& rotationDegree);

private:
    int m_width;
    int m_height;
    Vector3<float> m_cameraPos;
    Vector3<float> m_rotationDegree;

    // Windowing API specific objects (Easy to swap to Win32 later)
    SDL_Window* m_window;
    SDL_Renderer* m_sdlRenderer;
    SDL_Texture* m_sdlTexture;

    // Core Rasterizer components
    FrameBuffer& m_framebuffer;
    std::vector<TransformedVertex> m_transformedVertices;
	ThreadPool m_threadPool;
};
