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
        SlotMap<GameObjectWrapper, GameObjectStoreHandle> gameObjects;

    public:
        GameObjectStore();

        template<GameObjectBase TConcreteGameObject>
        TConcreteGameObject*
        create();

        void
        destroy(GameObject* pObject);

        GameObject*
        upgradeHandle(GameObjectStoreHandle handle);

        bool
        validateHandle(GameObjectStoreHandle handle);

        GameObjectStoreHandle
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
