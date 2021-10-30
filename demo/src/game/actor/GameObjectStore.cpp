#include "GameObjectStore.h"

using namespace lemon;
using namespace lemon::game;

void
GameObjectStore::destroy(GameObject* pObject)
{
    gameObjects.remove(pObject->getObjectDescriptor().storeHandle);
}

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

GameObjectInternalHandle
GameObjectStore::registerObject(GameObject* pObject, TypeInfo typeInfo)
{
    auto handle = gameObjects.insert(pObject);

    auto& desc = pObject->objectDescriptor;
    desc.typeInfo = &typeid(*pObject);
    desc.storeHandle = handle;

    return handle;
}
