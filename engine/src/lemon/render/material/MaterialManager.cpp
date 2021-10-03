#include <lemon/render/material/MaterialManager.h>
#include <cassert>

using namespace lemon::render;

static MaterialManager* gInstance;

MaterialManager::MaterialManager()
{
    assert(gInstance == nullptr);
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