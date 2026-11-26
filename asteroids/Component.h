#pragma once

class Actor;

class Component
{
public:
    explicit Component(Actor* owner, int updateOrder);
    virtual ~Component() = default;

    virtual void Update(float deltaTime);

    int GetUpdateOrder() const;

protected:
    Actor* owner_;

private:
    int updateOrder_;

};