#include "GameObject.h"

using namespace lemon;
using namespace lemon::game;

const GameObjectDescriptor&
GameObject::getObjectDescriptor() const
{
    return objectDescriptor;
}
