#include <SDL3/SDL.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <format>
#include <math.h>
#include <chrono>


struct Vector2 
{
    float x{0.0f};
    float y{0.0f};

    Vector2 operator+(const Vector2& rhs) const {return { x + rhs.x, y + rhs.y }; }
    Vector2 operator*(float scalar) const { return { x * scalar, y * scalar }; }
    Vector2& operator+=(const Vector2& rhs) 
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
};

class Actor;

class Component
{
public:
    explicit Component(Actor* owner) : owner_(owner) {}
    virtual ~Component() = default;

    virtual void ProcessInput(const bool* keys) {} 
    virtual void Update(float deltaTime) {} 
    virtual void Render(SDL_Renderer* renderer) {}

protected:
    Actor* owner_;

};

class Actor
{
public:
    explicit Actor(std::string name)
        : name_(std::move(name))
        , position_({ 400.0f, 300.0f })
        , rotation_(0.0f) {}

    virtual ~Actor()
    {
        std::cout << "Actor destroyed: " << name_ << std::endl; 
    }

    void AddComponent(std::unique_ptr<Component> component)
    {
        components_.push_back(std::move(component));
    }

    void ProcessInput(const bool* keys)
    {
        for (auto& comp : components_)
        {
            comp->ProcessInput(keys);
        }
    }

    void Update(float deltaTime)
    {
        for (auto& comp : components_)
        {
            comp->Update(deltaTime);
        }
    }

    void Render(SDL_Renderer* renderer)
    {
        for (auto& comp : components_)
        {
            comp->Render(renderer);
        }
    }

    const Vector2& GetPosition() const { return position_; }
    void SetPosition(Vector2& pos) { position_ = pos; }

    float GetRotation() const { return rotation_; }
    void SetRotation(float rad) { rotation_ = rad; }

    const std::string& GetName() const { return name_; }

private:
    std::string name_;
    Vector2 position_;
    float rotation_;
    std::vector<std::unique_ptr<Component>> components_;
};

class MoveComponent : public Component
{
public:
    MoveComponent(Actor* owner, float angularSpeed, float acceleration)
        : Component(owner)
        , angularSpeed_(angularSpeed)
        , accelerationRate_(acceleration)
        , rotationFactor_(0.0f)
        , isThrusting_(false)
        , velocity_{ 0.0f, 0.0f }
    {}

    void Update(float deltaTime) override
    {
        // Rotate: A/D set rotationFactor_ to -1/+1, angularSpeed_ is rad/s
        float rotation = owner_->GetRotation() + rotationFactor_ * angularSpeed_ * deltaTime;
        owner_->SetRotation(rotation);

        // Thrust: accelerate along the ship's nose (local +x, same as the renderer)
        if (isThrusting_)
        {
            Vector2 forward{ std::cos(rotation), std::sin(rotation) };
            velocity_ += forward * (accelerationRate_ * deltaTime);
        }

        velocity_ = velocity_ * 0.99f;

        // Clamp to a max speed so the ship stays controllable
        float speed = std::sqrt(velocity_.x * velocity_.x + velocity_.y * velocity_.y);
        if (speed > kMaxSpeed)
        {
            velocity_ = velocity_ * (kMaxSpeed / speed);
        }

        Vector2 newPos = owner_->GetPosition() + (velocity_ * deltaTime);

        // Wrap around the screen edges
        if (newPos.x < 0.0f)            newPos.x += kScreenWidth;
        else if (newPos.x > kScreenWidth)  newPos.x -= kScreenWidth;
        if (newPos.y < 0.0f)            newPos.y += kScreenHeight;
        else if (newPos.y > kScreenHeight) newPos.y -= kScreenHeight;

        owner_->SetPosition(newPos);
    }

    void SetRotationFactor(float rotation)
    {
        rotationFactor_ = rotation;
    }

    void SetThrusting(bool thrusting) { isThrusting_ = thrusting; }

private:
    static constexpr float kMaxSpeed = 400.0f;
    static constexpr float kScreenWidth = 800.0f;
    static constexpr float kScreenHeight = 600.0f;

    float angularSpeed_;
    float accelerationRate_;
    float rotationFactor_;
    bool isThrusting_;
    Vector2 velocity_;
};

class InputComponent : public Component
{
public:
    explicit InputComponent(Actor* owner)
        : Component(owner)
        , moveComponent_(nullptr)
    {}

    void SetMoveComponent(MoveComponent* moveComponent)
    {
        moveComponent_ = moveComponent;
    }

    void ProcessInput(const bool* keys) override
    {
        if (!moveComponent_) return;

        float rotFactor = 0.0f;
        if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
        {
            rotFactor = -1.0f;
        }
        if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
        {
            rotFactor = 1.0f;
        }
        moveComponent_->SetRotationFactor(rotFactor);

        bool thrusting = keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP];
        moveComponent_->SetThrusting(thrusting);
    }

private:
    MoveComponent* moveComponent_;
};

class ShipRenderComponent : public Component
{
public:
    explicit ShipRenderComponent(Actor* owner) : Component(owner) {}

    void Render(SDL_Renderer* renderer) override
    {
        Vector2 pos = owner_->GetPosition();
        float rot = owner_->GetRotation();

        auto transformPoint = [&](float lx, float ly) -> SDL_FPoint
        {
            float rx = lx * std::cos(rot) - ly * std::sin(rot);
            float ry = lx * std::sin(rot) + ly * std::cos(rot);
            return SDL_FPoint{ pos.x + rx, pos.y + ry };
        };

        SDL_FPoint p1 = transformPoint(  20.0f ,   0.0f);
        SDL_FPoint p2 = transformPoint( -10.0f , -10.0f);
        SDL_FPoint p3 = transformPoint( -10.0f ,  10.0f);

        SDL_SetRenderDrawColor(renderer, 0, 255, 128, 255);
        SDL_RenderLine(renderer, p1.x, p1.y, p2.x, p2.y);
        SDL_RenderLine(renderer, p2.x, p2.y, p3.x, p3.y);
        SDL_RenderLine(renderer, p3.x, p3.y, p1.x, p1.y);
    }
};

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        return -1;
    }

    using WindowPtr = std::unique_ptr<SDL_Window, decltype (&SDL_DestroyWindow)>;
    using RendererPtr = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;

    WindowPtr window(SDL_CreateWindow("Asteroids", 800, 600, 0), SDL_DestroyWindow);
    RendererPtr renderer(SDL_CreateRenderer(window.get(), NULL), SDL_DestroyRenderer);
    SDL_SetRenderVSync(renderer.get(), 1);

    Actor ship("PlayerShip");

    auto moveComp = std::make_unique<MoveComponent>(&ship, 4.0f, 300.0f);
    auto inputComp = std::make_unique<InputComponent>(&ship);
    auto renderComp = std::make_unique<ShipRenderComponent>(&ship);

    inputComp->SetMoveComponent(moveComp.get());

    ship.AddComponent(std::move(moveComp));
    ship.AddComponent(std::move(inputComp));
    ship.AddComponent(std::move(renderComp));

    bool isRunning = true;
    SDL_Event event;
    auto lastTime = std::chrono::high_resolution_clock::now();

    std::cout << "=== [Desafio Asteroids Iniciado] ===\n";

    while (isRunning)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT) isRunning = false;
        }

        const bool* keyState = SDL_GetKeyboardState(NULL);
        if (keyState[SDL_SCANCODE_ESCAPE]) isRunning = false;

        ship.ProcessInput(keyState);

        ship.Update(deltaTime);

        SDL_SetRenderDrawColor(renderer.get(), 15, 15, 20, 255);
        SDL_RenderClear(renderer.get());

        ship.Render(renderer.get());

        SDL_RenderPresent(renderer.get());
    }

    SDL_Quit();

    return 0;
}

