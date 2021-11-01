#pragma once

#include <lemon/game/actor/common.h>

namespace lemon::game {
    class GameObject;
    class TickGroup;
    struct TickProxy;

    template<class T>
    concept GameObjectBase = Base<T, GameObject> && std::is_default_constructible_v<T>;

    struct GameObjectDescriptor {
        TypeInfo typeInfo;
        GameObjectStoreHandle storeHandle;
    };

    struct GameObjectTickDescriptor {
    public:
        static constexpr size_t kMaxInlineTickDependencies = 4;

        using Dependencies = folly::small_vector<GameObject*, kMaxInlineTickDependencies>;

    private:
        TickGroup* pGroup;
        GameObject* pOwner;
        Dependencies dependencies;
        Dependencies dependants;
        TickProxyHandle handle;
        float interval;
        bool bEnabled;

    public:
        GameObjectTickDescriptor(GameObject* pOwner);

        ~GameObjectTickDescriptor();

        Dependencies&
        getDependencies();

        const Dependencies&
        getDependencies() const;

        void
        addDependency(GameObject* pObject);

        void
        removeDependency(GameObject* pObject);

        void
        addDependant(GameObject* pObject);

        void
        removeDependant(GameObject* pObject);

        void
        setTickingParent(TickProxyHandle handle);

        TickProxyHandle
        getHandle() const;

        float
        getInterval() const;

        void
        setInterval(float interval);

        TickGroup*
        getGroup();

        const TickGroup*
        getGroup() const;

        void
        setGroup(TickGroup* group);

        TickProxy*
        getProxy();

        const TickProxy*
        getProxy() const;

        void
        enable(float interval);

        void
        disable();

        bool
        isEnabled() const;

    private:
        void
        setHandle(TickProxyHandle handle);

        void
        updateProxy();
    };

    class GameObject : NonCopyable {
        static constexpr size_t kMaxInlineGameObjects = 8;

        friend class GameObjectStore;
        friend class GameWorld;

    public:
        using SubObjectList = folly::small_vector<GameObject*, kMaxInlineGameObjects>;

    private:
        GameObjectDescriptor objectDescriptor;
        GameObject* pParent{nullptr};
        SubObjectList subObjects{};
        std::string objectName{};

    protected:
        GameObjectTickDescriptor tick;

    public:
        GameObject();

        virtual ~GameObject();

        template<typename T>
        T*
        cast();

        template<typename T>
        const T*
        cast() const;

        template<typename T>
        T*
        cast(GameObject* pObject) const;

        template<typename T>
        const T*
        cast(const GameObject* pObject) const;

        const GameObjectDescriptor&
        getObjectDescriptor() const;

        template<GameObjectBase TConcreteGameObject>
        TConcreteGameObject*
        createSubObject();

        const SubObjectList&
        getSubObjectList() const;

        SubObjectList&
        getSubObjectList();

        const GameObject*
        getParent() const;

        GameObject*
        getParent();

        GameObjectStoreHandle
        getStoreHandle() const;

        void
        iterateSubObjects(const std::function<void(const GameObject*)>& fn, bool bRecursive = false) const;

        void
        iterateSubObjects(const std::function<void(GameObject*)>& fn, bool bRecursive = false);

        virtual void
        onStart();

        virtual void
        onTick(float deltaTime);

        virtual void
        onStop();

        virtual void
        enableTick(float interval = 0.f);

        virtual void
        disableTick();

        bool
        isTickEnabled() const;

        const GameObjectTickDescriptor&
        getTickDescriptor() const;

        GameObjectTickDescriptor&
        getTickDescriptor();

        bool
        isParentOf(const GameObject* pObject) const;

        void
        setName(const std::string& name);

        const std::string&
        getName() const;

        virtual void
        addTickDependency(GameObject* pOther);

        virtual void
        removeTickDependency(GameObject* pOther);

    protected:
        void
        setParent(GameObject* pParent);
    };

    template<GameObjectBase TConcreteGameObject>
    struct GameObjectHandle {
    private:
        GameObjectStoreHandle storeHandle;

    public:
        GameObjectHandle() = default;

        GameObjectHandle(GameObjectStoreHandle handle);

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

    template<typename T>
    inline T*
    GameObject::cast()
    {
        return ::lemon::game::cast<T>(this);
    }

    template<typename T>
    inline const T*
    GameObject::cast() const
    {
        return ::lemon::game::cast<T>(this);
    }

    template<typename T>
    inline T*
    GameObject::cast(GameObject* pObject) const
    {
        return ::lemon::game::cast<T>(pObject);
    }

    template<typename T>
    inline const T*
    GameObject::cast(const GameObject* pObject) const
    {
        return ::lemon::game::cast<T>(pObject);
    }

    template<GameObjectBase TConcreteGameObject>
    TConcreteGameObject*
    GameObject::createSubObject()
    {
        auto* pObject = new TConcreteGameObject();

        registerObject(pObject, getTypeInfo(pObject));

        pObject->setParent(this);
        subObjects.push_back(pObject);

        return pObject;
    }

    template<GameObjectBase TConcreteGameObject>
    GameObjectHandle<TConcreteGameObject>::GameObjectHandle(GameObjectStoreHandle handle)
        : storeHandle{handle}
    {
    }

    template<GameObjectBase TConcreteGameObject>
    GameObjectHandle<TConcreteGameObject>::GameObjectHandle(const GameObject* pObject)
    {
        LEMON_ASSERT(pObject->cast<TConcreteGameObject>() != nullptr);
        storeHandle = pObject->getObjectDescriptor().storeHandle;
    }

    template<GameObjectBase TConcreteGameObject>
    TConcreteGameObject*
    GameObjectHandle<TConcreteGameObject>::get()
    {
        return cast<TConcreteGameObject>(upgradeHandle(storeHandle));
    }

    template<GameObjectBase TConcreteGameObject>
    inline const TConcreteGameObject*
    GameObjectHandle<TConcreteGameObject>::get() const
    {
        return cast<TConcreteGameObject>(upgradeHandle(storeHandle));
    }

    template<GameObjectBase TConcreteGameObject>
    inline bool
    GameObjectHandle<TConcreteGameObject>::isValid() const
    {
        return validateHandle(storeHandle);
    }

    template<GameObjectBase TConcreteGameObject>
    inline GameObjectHandle<TConcreteGameObject>::operator bool() const
    {
        return isValid();
    }

    template<GameObjectBase TConcreteGameObject>
    inline GameObjectHandle<TConcreteGameObject>&
    GameObjectHandle<TConcreteGameObject>::operator=(const GameObject* pObject)
    {
        LEMON_ASSERT(pObject->cast<TConcreteGameObject>() != nullptr);
        storeHandle = pObject->getObjectDescriptor().storeHandle;
        return *this;
    }
} // namespace lemon::game
