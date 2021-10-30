#include "common.h"
#include "GameObjectStore.h"
#include "GameWorld.h"

using namespace lemon::game;

GameObject*
lemon::game::upgradeHandle(GameObjectInternalHandle handle)
{
    return GameWorld::getStoreInternal()->upgradeHandle(handle);
}

bool
lemon::game::validateHandle(GameObjectInternalHandle handle)
{
    return GameWorld::getStoreInternal()->validateHandle(handle);
}

TypeInfo
lemon::game::getTypeInfo(const GameObject* pObject)
{
    return &typeid(*pObject);
}

GameObjectInternalHandle
lemon::game::registerObject(GameObject* pObject, TypeInfo typeInfo)
{
    return GameWorld::getStoreInternal()->registerObject(pObject, typeInfo);
}
