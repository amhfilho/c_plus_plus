#include <SDL3/SDL.h>
#include <stdexcept>
#include <chrono>
#include <iostream>

struct SDLContext {
    SDL_Window* w;
    SDL_Renderer* r;
};

struct Vector2 {
    float x,y;
};

bool running = true;
Vector2 boxPosition = {0, 100};
auto lastTime = std::chrono::high_resolution_clock::now(); 
float timeAccumulator = 0.0f; // Acumula o tempo decorrido dos frames 
int frameCounter = 0; // Conta quantos frames rodaram no intervalo
int seconds = 60;
float speed = 200;

SDLContext InitSDL();
void DestroySDL(SDLContext ctx);
void GameLoop(SDLContext ctx);
void ProcessInput();
void UpdateGame();

void GenerateOutput(SDLContext ctx);

int main() {
    SDLContext ctx = InitSDL();
    
    GameLoop(ctx);

    DestroySDL(ctx);
}

SDLContext InitSDL() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Error: %s", SDL_GetError());
        throw std::runtime_error(SDL_GetError());
    }
    SDL_Window* w = SDL_CreateWindow("Modulo 1", 800, 600, 0);

    SDL_Renderer* r = SDL_CreateRenderer(w, nullptr);

    if (!SDL_SetRenderVSync(r, 1)) {
        SDL_Log("Unable to enable vertical sync: %s", SDL_GetError());
        throw std::runtime_error(SDL_GetError());
    }
    
    return SDLContext{w, r};
}

void DestroySDL(SDLContext ctx) {
    SDL_DestroyRenderer(ctx.r);
    SDL_DestroyWindow(ctx.w);
    SDL_Quit();
}
void GameLoop(SDLContext ctx) {
    while (running) {
        
        ProcessInput();
        UpdateGame();
        GenerateOutput(ctx);
    }
}

void ProcessInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            running = false;
        }
    }
}

void UpdateGame() {
    auto currentTime = std::chrono::high_resolution_clock::now(); 
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;

    timeAccumulator += deltaTime;
    frameCounter++;
    if (timeAccumulator >= 1.0f) {
        float avgFPS = static_cast<float>(frameCounter) / timeAccumulator;
        float avgDeltaTime = timeAccumulator / static_cast<float>(frameCounter);

        std::cout << "Average FPS: " << avgFPS << " - Average Delta Time: " << avgDeltaTime << std::endl;

        timeAccumulator = 0.0f;
        frameCounter = 0;
        seconds--;
    }

    boxPosition.x += speed * deltaTime;
    if (boxPosition.x > 800) {
        boxPosition.x = -30;
    }
}

void GenerateOutput(SDLContext ctx) {
    SDL_SetRenderDrawColor(ctx.r, 0, 0, 0, 0);
    SDL_RenderClear(ctx.r);
    SDL_SetRenderDrawColor(ctx.r, 255, 255, 255, 255);
    SDL_SetRenderScale(ctx.r, 3.0f, 3.0f);
    SDL_RenderDebugText(ctx.r, 10, 10, std::to_string(seconds).c_str());
    SDL_SetRenderScale(ctx.r, 1.0f, 1.0f);
    SDL_FRect rect = {boxPosition.x, boxPosition.y, 30, 30};
    SDL_RenderFillRect(ctx.r, &rect);

    SDL_RenderPresent(ctx.r);
}
