#include <SDL3/SDL.h>
#include <stdexcept>
#include <memory>
#include <iostream>

using WindowPtr = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using RendererPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;

class GraphicContext {
public:
    GraphicContext(const char* title, int width, int height)
        : window(SDL_CreateWindow(title, width, height, 0), SDL_DestroyWindow)
        , renderer(nullptr, SDL_DestroyRenderer)
    {
        if (!window) {
            throw std::runtime_error("Failed to create SDL window or renderer");
        }

        renderer.reset(SDL_CreateRenderer(window.get(), nullptr)); 
        if (!renderer) {
            throw std::runtime_error("Failed to create SDL renderer");
        }

        std::cout << "Graphic context successfully created." << std::endl;
    }

    ~GraphicContext() {
        std::cout << "Graphic context destroyed." << std::endl;
    }

    SDL_Renderer* GetRenderer() const {
        return renderer.get();
    }

private:
    WindowPtr window;
    RendererPtr renderer;
};

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    std::cout << "Initializing graphic context..." << std::endl;

    {
        GraphicContext context("Modulo 2 - RAII & Resource Management", 800, 600);

        SDL_SetRenderDrawColor(context.GetRenderer(), 40, 40, 80, 255);
        SDL_RenderClear(context.GetRenderer());
        SDL_RenderPresent(context.GetRenderer());

        std::cout << "Still inside the graphic context scope." << std::endl;
    }

    std::cout << "Exited the graphic context scope." << std::endl;

    SDL_Quit();

}