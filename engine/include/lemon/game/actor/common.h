#pragma once

namespace lemon::game {
    class GameObject;

    using TypeInfo = const std::type_info*;

    LEMON_SLOT_MAP_HANDLE(GameObjectStoreHandle);
    LEMON_SLOT_MAP_HANDLE(GameObjectWorldHandle);
    LEMON_SLOT_MAP_HANDLE(RenderProxyHandle);
    LEMON_SLOT_MAP_HANDLE(TickProxyHandle);

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
    upgradeHandle(GameObjectStoreHandle handle);

    bool
    validateHandle(GameObjectStoreHandle handle);

    TypeInfo
    getTypeInfo(const GameObject* pObject);

    // TODO: Make internal.
    GameObjectStoreHandle
    registerObject(GameObject* pObject, TypeInfo typeInfo);
} // namespace lemon::game
