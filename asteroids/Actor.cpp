#include "Actor.h"
#include "Component.h"

Actor::Actor(std::string name)
    : name_(std::move(name))
    , position_({0,0})
    , rotation_(0)
{
}

std::string &Actor::GetName()
{
    return name_;
}

void Actor::AddComponent(std::unique_ptr<Component> component)
{
    components_.push_back(std::move(component));
}

void Actor::Update(float deltaTime)
{
    for (auto& comp : components_)
    {
        comp->Update(deltaTime);
    }
}

Vector2 Actor::GetPosition() const
{
    return position_;
}

void Actor::SetPosition(Vector2 position)
{
    position_= position;
}

float Actor::GetRotation() const
{
    return rotation_;
}

void Actor::SetRotation(float rotation)
{
    rotation_ = rotation;
}
