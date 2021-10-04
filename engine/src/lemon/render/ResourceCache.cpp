#include <lemon/render/ResourceCache.h>
#include <lemon/shared/assert.h>

using namespace lemon::render;

static ResourceCache* gInstance;

ResourceCache::ResourceCache()
{
    LEMON_ASSERT(gInstance == nullptr);
    gInstance = this;
}

ResourceCache::~ResourceCache()
{
    gInstance = nullptr;
}

ResourceCache*
ResourceCache::get()
{
    LEMON_ASSERT(gInstance != nullptr);
    return gInstance;
}