#pragma once

#include <lemon/game/actor/common.h>
#include <lemon/game/actor/GameObject.h>

namespace lemon::game {
    struct TickProxy {
        GameObject* pObject;

        // Possible optimization: instead of saving handle to a ticking parent, we can swap parent and child
        // (possibly multiple times, with all children) in the tick queue to guarantee that parent will be
        // ticked before its children.
        TickProxyHandle tickingParent;

        double lastTickTime;
        float interval;
        uint32_t dependencyCount;

        TickProxy(GameObject* pObject, float interval, uint32_t dependencyCount);
    };

    class TickGroup {
        static constexpr size_t kBaseCapacity = 512;

        SlotMap<TickProxy, TickProxyHandle> proxies;
        double lastTickTime;

    public:
        TickGroup();

        TickProxyHandle
        add(const TickProxy& proxy);

        void
        remove(TickProxyHandle handle);

        void
        tick(double time);

        TickProxy*
        getProxy(TickProxyHandle handle);

    private:
        void
        tickImpl(TickProxy& proxy, float dt);
    };
} // namespace lemon::game
