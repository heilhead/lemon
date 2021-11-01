#pragma once

#include "GameObject.h"
#include "ActorComponent.h"

namespace lemon::game {
    class Actor;

    template<class T>
    concept ActorBase = Base<T, Actor> && GameObjectBase<T>;

    class Actor : public GameObject {
        friend class GameWorld;

        GameObjectWorldHandle worldHandle;
        bool bSealed = false;
        bool bComponentsInitialized = false;
        bool bAddedToWorld = false;

    protected:
        PositionableComponent* root;

    public:
        Actor();

        ~Actor() override;

        template<ActorComponentBase TComponent>
        TComponent*
        getComponent();

        template<ActorComponentBase TComponent, class TContainer>
        bool
        findComponents(TContainer& container);

        PositionableComponent*
        getRoot();

        const PositionableComponent*
        getRoot() const;

        virtual void
        onPreInitializeComponents();

        virtual void
        onPostInitializeComponents();

        void
        onStart() override;

        void
        onStop() override;

        void
        addTickDependency(GameObject* pOtherComponent) override;

        void
        removeTickDependency(GameObject* pOtherComponent) override;

    protected:
        template<ActorComponentBase TComponent>
        TComponent*
        addComponent(PositionableComponent* parentComponent = nullptr);

    private:
        void
        seal();

        void
        start();

        void
        stop();

        void
        initializeComponents();

        void
        uninitializeComponents();

        void
        unregisterComponents();

        template<ActorComponentBase TComponent>
        TComponent*
        getComponentRecursive(const GameObject* pCurrentObject);

        template<ActorComponentBase TComponent, class TContainer>
        void
        findComponentsRecursive(const GameObject* pCurrentObject, TContainer& container);
    };

    template<ActorComponentBase TComponent>
    inline TComponent*
    Actor::getComponent()
    {
        return getComponentRecursive<TComponent>(this);
    }

    template<ActorComponentBase TComponent, class TContainer>
    bool
    Actor::findComponents(TContainer& container)
    {
        auto initialSize = container.size();
        findComponentsRecursive<TComponent>(this, container);
        return container.size() != initialSize;
    }

    template<ActorComponentBase TComponent>
    TComponent*
    Actor::addComponent(PositionableComponent* pParentComponent)
    {
        LEMON_ASSERT(!bSealed, "components can be added only during actor construction");

        static constexpr auto bPositionable = std::is_base_of_v<PositionableComponent, TComponent>;
        LEMON_ASSERT(pParentComponent == nullptr || bPositionable,
                     "only positionable components can be nested into other components");

        auto* pComponent = pParentComponent == nullptr ? createSubObject<TComponent>()
                                                       : pParentComponent->createSubObject<TComponent>();
        pComponent->setOwner(this);
        pComponent->onRegister();

        return pComponent;
    }

    template<ActorComponentBase TComponent>
    TComponent*
    Actor::getComponentRecursive(const GameObject* pCurrentObject)
    {
        for (auto* pObject : pCurrentObject->getSubObjectList()) {
            if (auto* pComponent = cast<TComponent>(pObject)) {
                return pComponent;
            }

            if (auto* pComponent = getComponentRecursive<TComponent>(pObject)) {
                return pComponent;
            }
        }

        return nullptr;
    }

    template<ActorComponentBase TComponent, class TContainer>
    void
    Actor::findComponentsRecursive(const GameObject* pCurrentObject, TContainer& container)
    {
        for (auto* pObject : pCurrentObject->getSubObjectList()) {
            if (auto* pComponent = cast<TComponent>(pObject)) {
                container.push_back(pComponent);
                findComponentsRecursive<TComponent>(pComponent, container);
            }
        }
    }
} // namespace lemon::game
