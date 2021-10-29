#pragma once

#include "GameObject.h"

namespace lemon::game {
    class ActorComponent;

    template<class T>
    concept ActorComponentBase = Base<T, ActorComponent> && GameObjectBase<T>;

    class ActorComponent : public GameObject {
        friend class Actor;

    public:
        ActorComponent() : GameObject()
        {
            LEMON_TRACE_FN();

            tick.setTickType(GameObjectTickType::Component);
        }

        virtual ~ActorComponent();

    protected:
        virtual void
        onRegister()
        {
            LEMON_TRACE_FN();
        }

        virtual void
        onInitialize()
        {
            LEMON_TRACE_FN();
        }

        virtual void
        onUninitialize()
        {
            LEMON_TRACE_FN();
        }

        virtual void
        onUnregister()
        {
            LEMON_TRACE_FN();
        }
    };
} // namespace lemon::game
