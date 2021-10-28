#include "common.h"
#include "GameObjectStore.h"

using namespace lemon::game;

GameObject*
lemon::game::upgradeHandle(GameObjectInternalHandle handle)
{
    return GameObjectStore::get()->upgradeHandle(handle);
}

bool
lemon::game::validateHandle(GameObjectInternalHandle handle)
{
    return GameObjectStore::get()->validateHandle(handle);
}
