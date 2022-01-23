#include <lemon/game/actor/TickGroup.h>

using namespace lemon;
using namespace lemon::game;

TickProxy::TickProxy(GameObject* pObject, float interval, uint32_t dependencyCount)
    : pObject{pObject}, interval{interval}, lastTickTime{0.f}, dependencyCount{dependencyCount}
{
}

TickGroup::TickGroup() : proxies{kBaseCapacity}, lastTickTime{0.f} {}

TickProxyHandle
TickGroup::add(const TickProxy& proxy)
{
    return proxies.insert(proxy);
}

void
TickGroup::remove(TickProxyHandle handle)
{
    proxies.remove(handle);
}

void
TickGroup::tick(double time)
{
    OPTICK_EVENT();

    auto dt = static_cast<float>(time - lastTickTime);
    lastTickTime = time;

    for (auto& proxy : proxies) {
        OPTICK_EVENT("TickProxy");

        tickImpl(proxy, dt);
    }
}

TickProxy*
TickGroup::getProxy(TickProxyHandle handle)
{
    return proxies.getData(handle);
}

inline void
TickGroup::tickImpl(TickProxy& proxy, float dt)
{
    bool bShouldTick =
        (lastTickTime != proxy.lastTickTime) && (lastTickTime - proxy.lastTickTime >= proxy.interval);

    if (!bShouldTick) {
        return;
    }

    if (!proxy.tickingParent.isEmpty()) {
        tickImpl(proxies[proxy.tickingParent], dt);
    }

    if (proxy.dependencyCount > 0) {
        for (auto* pDepObject : proxy.pObject->getTickDescriptor().getDependencies()) {
            // The object listed as a dependency may not be ticking, and hence doesn't have a handle.
            if (auto* pDepProxy = proxies.getData(pDepObject->getTickDescriptor().getHandle())) {
                tickImpl(*pDepProxy, dt);
            }
        }
    }

    proxy.pObject->onTick(dt);
    proxy.lastTickTime = lastTickTime;
}
