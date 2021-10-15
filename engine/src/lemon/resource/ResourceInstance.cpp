#include <lemon/resource/ResourceInstance.h>

using namespace lemon::res;

ResourceInstance::~ResourceInstance()
{
    for (ResourceObject* pObj : objects) {
        delete pObj;
    }

    detachDependencies(nullptr);
}

void
ResourceInstance::addDependency(ResourceInstance* pResource)
{
    dependencies.push_back(pResource);
    pResource->addDependant(/*this*/);
}

void
ResourceInstance::detachDependencies(std::vector<ResourceHandle>* unusedDeps)
{
    for (ResourceInstance* pResource : dependencies) {
        pResource->removeDependant(/*this*/);

        if (unusedDeps != nullptr && pResource->getDependantCount() == 0) {
            unusedDeps->push_back(pResource->getHandle());

            pResource->detachDependencies(unusedDeps);
        }
    }

    dependencies.clear();
}
