#pragma once

#include "common.h"
#include "GameObject.h"

namespace lemon::game {
    class TickGroup {
        static constexpr size_t kBaseCapacity = 512;

        SlotMap<GameObjectTickProxy, TickGroupHandle> proxies;
        double lastTickTime;

    public:
        TickGroup();

        TickGroupHandle
        add(const GameObjectTickProxy& proxy);

        void
        remove(TickGroupHandle handle);

        void
        tick(double time);

        GameObjectTickProxy*
        getProxy(TickGroupHandle handle);

    private:
        void
        tickImpl(GameObjectTickProxy& proxy, float dt);
    };
} // namespace lemon::game
