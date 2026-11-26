#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Vector2.h"

class Component;

class Actor
{
public:
    explicit Actor(std::string name);
    virtual ~Actor() = default; 

    std::string& GetName();

    void AddComponent(std::unique_ptr<Component> component);

    void Update(float deltaTime);

    Vector2 GetPosition() const;
    void SetPosition(Vector2 position);

    float GetRotation() const;
    void SetRotation(float rotation);


private:
    std::string name_;
    Vector2 position_;
    float rotation_;
    float speed_;
    std::vector<std::unique_ptr<Component>> components_;
};