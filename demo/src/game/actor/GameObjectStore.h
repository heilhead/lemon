#pragma once

#include "common.h"
#include "GameObject.h"

namespace lemon::game {
    struct GameObjectWrapper {
        std::unique_ptr<GameObject> gameObject{nullptr};

        explicit GameObjectWrapper(GameObject* pObject)
        {
            gameObject = std::unique_ptr<GameObject>(pObject);
        }
    };

    class GameObjectStore : public UnsafeSingleton<GameObjectStore> {
        static constexpr size_t kMaxAliveGameObjects = 16384;

        SlotMap<GameObjectWrapper, kMaxAliveGameObjects> gameObjects;

    public:
        GameObjectStore() {}

        template<DerivedFrom<GameObject> TGameObject>
        TGameObject*
        create()
        {
            auto* pObject = new TGameObject();
            auto handle = gameObjects.insert(pObject);

            GameObjectDescriptor desc{.typeInfo = &typeid(*pObject), .storeHandle = handle};
            pObject->objectDescriptor = std::move(desc);

            return pObject;
        }

        GameObject*
        upgradeHandle(GameObjectInternalHandle handle);

        bool
        validateHandle(GameObjectInternalHandle handle);
    };
} // namespace lemon::game
