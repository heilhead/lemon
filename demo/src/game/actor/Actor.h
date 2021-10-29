#pragma once

#include "GameObject.h"
#include "ActorComponent.h"

namespace lemon::game {
    class Actor;

    template<class T>
    concept ActorBase = Base<T, Actor> && GameObjectBase<T>;

    class Actor : public GameObject {
        bool bComponentsInitialized = false;
        bool bTickEnabled = false;

    public:
        Actor() : GameObject()
        {
            LEMON_TRACE_FN();

            tick.setTickType(GameObjectTickType::Actor);
        }

        ~Actor() override;

        template<ActorComponentBase TComponent>
        TComponent*
        addComponent();

        template<ActorComponentBase TComponent>
        TComponent*
        getComponent();

        virtual void
        onPreInitializeComponents()
        {
            LEMON_TRACE_FN();
        }

        virtual void
        onPostInitializeComponents()
        {
            LEMON_TRACE_FN();
        }

        // INTERNAL START
        void
        onStartInternal();

        void
        onStopInternal();
        // INTERNAL END

    private:
        void
        initializeComponents();

        void
        uninitializeComponents();

        void
        unregisterComponents();
    };

    template<ActorComponentBase TComponent>
    TComponent*
    Actor::addComponent()
    {
        auto* pComponent = createSubObject<TComponent>();
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
