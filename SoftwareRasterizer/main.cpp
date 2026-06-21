#include <SDL3/SDL.h>
#include <vector>
#include <cstdint>

int main(int argc, char* argv[])
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Software Rasterizer", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT);

    // Your CPU-side framebuffer — this is what you'll draw into
    std::vector<uint32_t> framebuffer(WIDTH * HEIGHT, 0xFF000000);

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_EVENT_QUIT) running = false;

        // --- YOUR RENDERING CODE GOES HERE ---
        // Write pixels into framebuffer like:
        // framebuffer[y * WIDTH + x] = 0xFFRRGGBB;

        // Draw a test pixel in the center
        framebuffer[(HEIGHT / 2) * WIDTH + (WIDTH / 2)] = 0xFFFF0000; // red

        // Blit framebuffer to screen
        SDL_UpdateTexture(texture, nullptr, framebuffer.data(), WIDTH * sizeof(uint32_t));
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}