#include <lemon/resource/ResourceInstance.h>

using namespace lemon::res;

ResourceInstance::~ResourceInstance() {
    for (ResourceObject* pObj : objects) {
        delete pObj;
    }

    for (ResourceInstance* pResource : dependencies) {
        pResource->removeDependant(/*this*/);
    }
}

void
ResourceInstance::addDependency(ResourceInstance* pResource) {
    dependencies.push_back(pResource);
    pResource->addDependant(/*this*/);
}