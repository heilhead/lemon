#include <lemon/render/material/MaterialManager.h>
#include <lemon/shared/assert.h>

using namespace lemon::render;

static MaterialManager* gInstance;

MaterialManager::MaterialManager()
{
    LEMON_ASSERT(gInstance == nullptr);
    gInstance = this;
}

MaterialManager::~MaterialManager()
{
    gInstance = nullptr;
}

MaterialManager*
MaterialManager::get()
{
    return gInstance;
}