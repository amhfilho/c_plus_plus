#include <string>
#include <iostream>
#include <vector>
#include <memory>

class Actor;

class Component
{
public:
    explicit Component(Actor* owner, int updateOrder = 100)
                : owner_(owner)
                , updateOrder_(updateOrder) { }
    virtual ~Component() = default;

    virtual void ProcessInput() {}
    virtual void Update(float deltaTime) {}

    int GetUpdateOrder() const { return updateOrder_; }

protected:
    Actor* owner_;
    
private:
    int updateOrder_;
};

class MoveComponent : public Component
{
public:
    MoveComponent(Actor* owner, float speed)
        : Component(owner)
        , speed_(speed) { }

    ~MoveComponent() override
    {
        std::cout << "Destroying component" << std::endl;
    }

    void Update(float deltaTime) override;

private:
    float speed_;

};

class Actor
{
public:
    explicit Actor(std::string name) : name_(std::move(name)) { }

    virtual ~Actor() 
    {
        std::cout << "Actor destroyed"  << std::endl;    
    }

    void AddComponent(std::unique_ptr<Component> component)
    {
        std::cout << "Adding component" << std::endl;
        components_.push_back(std::move(component));
    }

    void Update(float deltaTime)
    {
        for (auto& comp : components_)
        {
            comp->Update(deltaTime);
        }
    }

    const std::string& GetName() { return name_; }

private:
    std::string name_;
    std::vector<std::unique_ptr<Component>> components_;
};

void MoveComponent::Update(float deltaTime)
{
    std::cout << "Updating Actor " << owner_->GetName() << ", speed: " << speed_ << " , deltaTime: " << deltaTime <<  std::endl;
}

int main()
{
    std::cout << "Modulo 3: Teste Actor / Component Architecture" << std::endl;

    {
        Actor player("NaveJogador");

        auto moveComponent = std::make_unique<MoveComponent>(&player, 150.0f);
        player.AddComponent(std::move(moveComponent));

        std::cout << "Simulating 2 frames..." << std::endl;
        player.Update(0.016f);
        player.Update(0.016f);


        std::cout << "Exiting scope block" << std::endl;
    }

    std::cout << "Test finished with success" << std::endl;

}