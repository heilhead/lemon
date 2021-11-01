#include <lemon/game/actor/common.h>
#include <lemon/game/actor/GameObjectStore.h>
#include <lemon/game/actor/GameWorld.h>

using namespace lemon::game;

GameObject*
lemon::game::upgradeHandle(GameObjectStoreHandle handle)
{
    return GameWorld::getStoreInternal()->upgradeHandle(handle);
}

bool
lemon::game::validateHandle(GameObjectStoreHandle handle)
{
    return GameWorld::getStoreInternal()->validateHandle(handle);
}

TypeInfo
lemon::game::getTypeInfo(const GameObject* pObject)
{
    return &typeid(*pObject);
}

GameObjectStoreHandle
lemon::game::registerObject(GameObject* pObject, TypeInfo typeInfo)
{
    return GameWorld::getStoreInternal()->registerObject(pObject, typeInfo);
}
