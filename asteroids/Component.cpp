#include <iostream>
#include "Component.h"

class Actor;

Component::Component(Actor *owner, int updateOrder = 100)
    : owner_(owner)
    , updateOrder_(updateOrder) { };

Component::~Component() 
{
    std::cout << "Component destroyed" << std::endl;
}

int Component::GetUpdateOrder() const
{
    return updateOrder_;
}