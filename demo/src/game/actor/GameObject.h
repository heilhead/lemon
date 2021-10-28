#pragma once

#include "common.h"

namespace lemon::game {
    class GameObject;

    using TypeInfo = const std::type_info*;

    struct GameObjectDescriptor {
        TypeInfo typeInfo;
        GameObjectInternalHandle storeHandle;
    };

    template<Base<GameObject> TConcreteGameObject>
    struct GameObjectHandle {
    private:
        GameObjectInternalHandle storeHandle;

    public:
        GameObjectHandle() = default;

        GameObjectHandle(GameObjectInternalHandle handle);

        GameObjectHandle(const GameObject* pObject);

        TConcreteGameObject*
        get();

        const TConcreteGameObject*
        get() const;

        bool
        isValid() const;

        operator bool() const;

        GameObjectHandle&
        operator=(const GameObject* pObject);
    };

    class GameObject : NonCopyable {
        friend class GameObjectStore;

        GameObjectDescriptor objectDescriptor;

    public:
        GameObject() {}

        virtual ~GameObject() = default;

        template<Base<GameObject> TConcreteGameObject>
        inline TConcreteGameObject*
        cast();

        template<Base<GameObject> TConcreteGameObject>
        inline const TConcreteGameObject*
        cast() const;

        const GameObjectDescriptor&
        getObjectDescriptor() const;
    };

    template<Base<GameObject> TConcreteGameObject>
    inline TConcreteGameObject*
    cast(GameObject* pObject)
    {
        return dynamic_cast<TConcreteGameObject*>(pObject);
    }

    template<Base<GameObject> TConcreteGameObject>
    inline const TConcreteGameObject*
    cast(const GameObject* pObject)
    {
        return dynamic_cast<const TConcreteGameObject*>(pObject);
    }

    template<Base<GameObject> TConcreteGameObject>
    GameObjectHandle<TConcreteGameObject>::GameObjectHandle(GameObjectInternalHandle handle)
        : storeHandle{handle}
    {
    }

    template<Base<GameObject> TConcreteGameObject>
    GameObjectHandle<TConcreteGameObject>::GameObjectHandle(const GameObject* pObject)
    {
        LEMON_ASSERT(pObject->cast<TConcreteGameObject>() != nullptr);
        storeHandle = pObject->getObjectDescriptor().storeHandle;
    }

    template<Base<GameObject> TConcreteGameObject>
    TConcreteGameObject*
    GameObjectHandle<TConcreteGameObject>::get()
    {
        return cast<TConcreteGameObject>(upgradeHandle(storeHandle));
    }

    template<Base<GameObject> TConcreteGameObject>
    inline const TConcreteGameObject*
    GameObjectHandle<TConcreteGameObject>::get() const
    {
        return cast<TConcreteGameObject>(upgradeHandle(storeHandle));
    }

    template<Base<GameObject> TConcreteGameObject>
    inline bool
    GameObjectHandle<TConcreteGameObject>::isValid() const
    {
        return validateHandle(storeHandle);
    }

    template<Base<GameObject> TConcreteGameObject>
    inline GameObjectHandle<TConcreteGameObject>::operator bool() const
    {
        return isValid();
    }

    template<Base<GameObject> TConcreteGameObject>
    inline GameObjectHandle<TConcreteGameObject>&
    GameObjectHandle<TConcreteGameObject>::operator=(const GameObject* pObject)
    {
        LEMON_ASSERT(pObject->cast<TConcreteGameObject>() != nullptr);
        storeHandle = pObject->getObjectDescriptor().storeHandle;
        return *this;
    }

    template<Base<GameObject> TConcreteGameObject>
    inline TConcreteGameObject*
    GameObject::cast()
    {
        return ::lemon::game::cast<TConcreteGameObject>(this);
    }

    template<Base<GameObject> TConcreteGameObject>
    inline const TConcreteGameObject*
    GameObject::cast() const
    {
        return ::lemon::game::cast<TConcreteGameObject>(this);
    }
} // namespace lemon::game
