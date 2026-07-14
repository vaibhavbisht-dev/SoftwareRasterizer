#include <SDL3/SDL.h>
#include <vector>
#include <cstdint>
#include "Renderer/Renderer.h"
#include "FrameBuffer.h"
#include "helper/SoftwareTexture.h"
#include "helper/Parser.h"

int main(int argc, char* argv[]) {
    const int WIDTH = 1920;
    const int HEIGHT = 1080;

    // Instantiate CPU pixel canvas context
    FrameBuffer framebuffer(WIDTH, HEIGHT);

    // Pass canvas reference to manage pipeline operations separately
    Renderer renderer(WIDTH, HEIGHT, framebuffer);
    if (!renderer.Initialize()) {
		std::cerr << "Failed to initialize renderer." << std::endl;
        return -1;
    }

    // Load geometry assets outside window updates
    OBJParser parser("./assets/car.obj");
    SoftwareTexture myTexture("./assets/car_texture.png");

    ObjectData objData = parser.ParseOBJ();
    std::vector<Vertex> vertices = objData.vertices;
    std::vector<Triangle> triangles = objData.triangles;

    bool running = true;
    SDL_Event event;
    Uint64 previousTime = SDL_GetTicks();

    // Polling and application main execution loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - previousTime) / 1000.0f;
        previousTime = currentTime;

        // Delegate entire scene transformation and rasterization pipeline
        renderer.RenderFrame(vertices, triangles, myTexture, deltaTime);
    }

    return 0;
}
