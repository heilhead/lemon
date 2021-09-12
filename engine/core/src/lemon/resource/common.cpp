#include <lemon/resource/common.h>
#include <lemon/resource/ResourceManager.h>

using namespace lemon::res;

ResourceState ResourceHandle::getState(ResourceObjectHandle object) {
    return ResourceManager::get()->getResourceState(*this, object);
}
