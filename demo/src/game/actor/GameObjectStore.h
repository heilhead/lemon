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

    // TODO: Make internal and accessible only to `GameWorld`. It probably shouldn't be a singleton and
    // instead live inside `GameWorld`.
    class GameObjectStore : public UnsafeSingleton<GameObjectStore> {
        SlotMap<GameObjectWrapper, kMaxAliveGameObjects> gameObjects;

    public:
        GameObjectStore() {}

        template<GameObjectBase TConcreteGameObject>
        TConcreteGameObject*
        create()
        {
            auto* pObject = new TConcreteGameObject();

            registerObject(pObject, getTypeInfo(pObject));

            return pObject;
        }

        void
        destroy(GameObject* pObject)
        {
            gameObjects.remove(pObject->getObjectDescriptor().storeHandle);
        }

        GameObject*
        upgradeHandle(GameObjectInternalHandle handle);

        bool
        validateHandle(GameObjectInternalHandle handle);

        GameObjectInternalHandle
        registerObject(GameObject* pObject, TypeInfo typeInfo);
    };
} // namespace lemon::game
