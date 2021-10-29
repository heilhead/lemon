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

        // TODO: Make internal
        GameObjectInternalHandle
        registerObject(GameObject* pObject, TypeInfo typeInfo);
    };
} // namespace lemon::game
