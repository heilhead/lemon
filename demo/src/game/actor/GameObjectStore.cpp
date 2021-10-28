#include "GameObjectStore.h"

using namespace lemon;
using namespace lemon::game;

GameObject*
GameObjectStore::upgradeHandle(GameObjectInternalHandle handle)
{
    if (gameObjects.isValid(handle)) {
        return gameObjects[handle].gameObject.get();
    }

    return nullptr;
}

bool
GameObjectStore::validateHandle(GameObjectInternalHandle handle)
{
    return gameObjects.isValid(handle);
}
