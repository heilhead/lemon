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
        bool bComponentsInitialized = false;
        bool bTickEnabled = false;
        bool bAddedToWorld = false;

    protected:
        PositionableComponent* rootComponent;

    public:
        Actor();

        ~Actor() override;

        template<ActorComponentBase TComponent>
        TComponent*
        addComponent(PositionableComponent* parentComponent = nullptr);

        template<ActorComponentBase TComponent>
        TComponent*
        getComponent();

        PositionableComponent*
        getRootComponent();

        virtual void
        onPreInitializeComponents();

        virtual void
        onPostInitializeComponents();

        void
        onStart() override;

        void
        onStop() override;

    private:
        void
        startInternal();

        void
        stopInternal();

        void
        initializeComponents();

        void
        uninitializeComponents();

        void
        unregisterComponents();
    };

    template<ActorComponentBase TComponent>
    TComponent*
    Actor::addComponent(PositionableComponent* parentComponent)
    {
        auto* pComponent = createSubObject<TComponent>();
        pComponent->setParent(parentComponent);
        pComponent->onRegister();
        return pComponent;
    }

    template<ActorComponentBase TComponent>
    TComponent*
    Actor::getComponent()
    {
        for (auto* pObject : getSubObjectList()) {
            if (auto* pComponent = cast<TComponent>(pObject)) {
                return pComponent;
            }
        }

        return nullptr;
    }
} // namespace lemon::game
