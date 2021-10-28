#pragma once

namespace lemon::game {
    class GameObject;

    using GameObjectInternalHandle = SlotMapHandle;

    GameObject*
    upgradeHandle(GameObjectInternalHandle handle);

    bool
    validateHandle(GameObjectInternalHandle handle);
} // namespace lemon::game
