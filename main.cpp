#include <chrono>
#include <iostream>
#include <thread>
#include <format> // Recurso do C++20 para formatação de texto

// Estrutura de vetor 2D alocada na Stack
struct Vector2 {
    float x{ 0.0f };
    float y{ 0.0f };
};

int main() {
    std::cout << "=== [C++20 Engine Loop Test] ===\n";

    Vector2 position{ 0.0f, 0.0f };       // Alocado na Stack (rápido, sem heap/GC)
    Vector2 velocity{ 100.0f, 50.0f };    // 100 px/s em X, 50 px/s em Y

    auto lastTime = std::chrono::high_resolution_clock::now();
    bool isRunning = true;
    int frameCount = 0;

    while (isRunning && frameCount < 5) {
        // 1. Cálculo do Delta Time (dt) usando C++20 chrono
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // 2. Update do Jogo: Posição = Posição + (Velocidade * dt)
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;

        // Imprime relatório no terminal com std::format (C++20)
        float fps = (deltaTime > 0.0f) ? (1.0f / deltaTime) : 0.0f;
        std::cout << std::format("[Frame {}] dt: {:.6f}s | FPS: {:.1f} | Pos: ({:.2f}, {:.2f})\n",
                                 frameCount + 1, deltaTime, fps, position.x, position.y);

        // Simula carga de trabalho do frame (~16.6ms para simular ~60 FPS)
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        frameCount++;
    }

    std::cout << "=== Loop encerrado com sucesso ===\n";
    return 0;
}