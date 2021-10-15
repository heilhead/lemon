#include <lemon/resource/ResourceManager.h>
#include <lemon/resource/common.h>

using namespace lemon::res;

ResourceState
ResourceHandle::getState(ResourceObjectHandle object)
{
    return ResourceManager::get()->getResourceState(*this, object);
}
