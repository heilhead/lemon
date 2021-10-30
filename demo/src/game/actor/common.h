#pragma once

namespace lemon::game {
    class GameObject;

    using TypeInfo = const std::type_info*;

    LEMON_SLOT_MAP_HANDLE(GameObjectInternalHandle);
    LEMON_SLOT_MAP_HANDLE(GameObjectWorldHandle);
    LEMON_SLOT_MAP_HANDLE(GameObjectTickProxyHandle);
    LEMON_SLOT_MAP_HANDLE(GameObjectRenderProxyHandle);

    static constexpr size_t kMaxAliveGameObjects = 16384;

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
