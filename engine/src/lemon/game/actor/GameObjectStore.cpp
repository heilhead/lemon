#include <lemon/game/actor/GameObjectStore.h>

using namespace lemon;
using namespace lemon::game;

GameObjectStore::GameObjectStore() : gameObjects{kMaxAliveGameObjects} {}

void
GameObjectStore::destroy(GameObject* pObject)
{
    gameObjects.remove(pObject->getObjectDescriptor().storeHandle);
}

GameObject*
GameObjectStore::upgradeHandle(GameObjectStoreHandle handle)
{
    if (gameObjects.isValid(handle)) {
        return gameObjects[handle].gameObject.get();
    }

    return nullptr;
}

bool
GameObjectStore::validateHandle(GameObjectStoreHandle handle)
{
    return gameObjects.isValid(handle);
}

GameObjectStoreHandle
GameObjectStore::registerObject(GameObject* pObject, TypeInfo typeInfo)
{
    auto handle = gameObjects.insert(pObject);

    auto& desc = pObject->objectDescriptor;
    desc.typeInfo = &typeid(*pObject);
    desc.storeHandle = handle;

    return handle;
}
