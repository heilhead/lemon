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

    class GameObjectStore {
        SlotMap<GameObjectWrapper, kMaxAliveGameObjects, GameObjectInternalHandle> gameObjects;

    public:
        GameObjectStore() {}

        template<GameObjectBase TConcreteGameObject>
        TConcreteGameObject*
        create();

        void
        destroy(GameObject* pObject);

        GameObject*
        upgradeHandle(GameObjectInternalHandle handle);

        bool
        validateHandle(GameObjectInternalHandle handle);

        GameObjectInternalHandle
        registerObject(GameObject* pObject, TypeInfo typeInfo);
    };

    template<GameObjectBase TConcreteGameObject>
    TConcreteGameObject*
    GameObjectStore::create()
    {
        auto* pObject = new TConcreteGameObject();

        registerObject(pObject, getTypeInfo(pObject));

        return pObject;
    }
} // namespace lemon::game
