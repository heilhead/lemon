#include "TickGroup.h"

using namespace lemon;
using namespace lemon::game;

TickGroup::TickGroup() : proxies{kBaseCapacity}, lastTickTime{0.f} {}

TickGroupHandle
TickGroup::add(const GameObjectTickProxy& proxy)
{
    return proxies.insert(proxy);
}

void
TickGroup::remove(TickGroupHandle handle)
{
    proxies.remove(handle);
}

void
TickGroup::tick(double time)
{
    auto dt = static_cast<float>(time - lastTickTime);
    lastTickTime = time;

    for (size_t i = 0, length = proxies.getSize(); i < length; i++) {
        tickImpl(proxies[i], dt);
    }
}

GameObjectTickProxy*
TickGroup::getProxy(TickGroupHandle handle)
{
    return proxies.getData(handle);
}

void inline TickGroup::tickImpl(GameObjectTickProxy& proxy, float dt)
{
    bool bShouldTick =
        (lastTickTime != proxy.lastTickTime) && (lastTickTime - proxy.lastTickTime >= proxy.interval);

    if (!bShouldTick) {
        return;
    }

    if (proxy.dependencyCount > 0) {
        auto& tickDesc = proxy.pObject->getTickDescriptor();

        for (auto depHandle : tickDesc.getDependencies()) {
            if (auto* pDepProxy = proxies.getData(depHandle)) {
                tickImpl(*pDepProxy, dt);
            }
        }
    }

    proxy.pObject->onTick(dt);
    proxy.lastTickTime = lastTickTime;
}
