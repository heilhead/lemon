#pragma once

#include "common.h"

namespace lemon::game {
    class GameObject;

    template<class T>
    concept GameObjectBase = Base<T, GameObject> && std::is_default_constructible_v<T>;

    struct GameObjectDescriptor {
        TypeInfo typeInfo;
        GameObjectInternalHandle storeHandle;
    };

    enum class GameObjectTickType { Actor, Component };

    struct GameObjectTickDescriptor {
    public:
        static constexpr size_t kMaxInlineTickDependencies = 4;

        using ProxyHandle = GameObjectTickProxyHandle;

        // TODO: Store a wrapper instead of raw handle which will include a 'user added' flag to make handles
        // permanent for user added dependencies.
        using Dependencies = folly::small_vector<ProxyHandle, kMaxInlineTickDependencies>;

    private:
        Dependencies dependencies{};
        GameObjectTickType tickType{};
        ProxyHandle tickProxyHandle{};
        float interval{0.f};

    public:
        Dependencies&
        getDependencies();

        const Dependencies&
        getDependencies() const;

        void
        addDependency(ProxyHandle handle);

        void
        removeDependency(ProxyHandle handle);

        GameObjectTickType
        getTickType() const;

        void
        setTickType(GameObjectTickType inTickType);

        void
        setHandle(ProxyHandle handle);

        ProxyHandle
        getHandle() const;

        float
        getInterval() const;

        void
        setInterval(float interval);
    };

    struct GameObjectTickProxy {
        GameObject* pObject;
        double lastTickTime;
        float interval;
        uint32_t dependencyCount;

        GameObjectTickProxy(GameObject* pObject, float interval);
    };

    class GameObject : NonCopyable {
        static constexpr size_t kMaxInlineGameObjects = 8;

        friend class GameObjectStore;
        friend class GameWorld;

    public:
        using SubObjectList = folly::small_vector<GameObject*, kMaxInlineGameObjects>;

    private:
        GameObjectDescriptor objectDescriptor;
        GameObject* pParent;
        SubObjectList subObjects;

    protected:
        GameObjectTickDescriptor tick;
        bool bTickEnabled = false;

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

        GameObjectInternalHandle
        getInternalHandle() const;

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

        void
        addTickDependencyInternal(GameObjectTickProxyHandle handle);

        void
        removeTickDependencyInternal(GameObjectTickProxyHandle handle);

        bool
        isParentOf(const GameObject* pObject) const;

    protected:
        void
        setParent(GameObject* pParent);

    private:
        GameObjectTickProxy
        createTickProxy();

        void
        updateTickProxy();
    };

    template<GameObjectBase TConcreteGameObject>
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
    GameObjectHandle<TConcreteGameObject>::GameObjectHandle(GameObjectInternalHandle handle)
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
