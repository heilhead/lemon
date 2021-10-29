#pragma once

namespace lemon::game {
    class GameObject;

    using TypeInfo = const std::type_info*;

    using GameObjectInternalHandle = SlotMapHandle;

    template<typename T>
    inline T*
    cast(GameObject* pObject)
    {
        return dynamic_cast<T*>(pObject);
    }

    template<typename T>
    inline const T*
    cast(const GameObject* pObject)
    {
        return dynamic_cast<const T*>(pObject);
    }

    GameObject*
    upgradeHandle(GameObjectInternalHandle handle);

    bool
    validateHandle(GameObjectInternalHandle handle);

    TypeInfo
    getTypeInfo(const GameObject* pObject);

    // TODO: Make internal.
    GameObjectInternalHandle
    registerObject(GameObject* pObject, TypeInfo typeInfo);
} // namespace lemon::game
