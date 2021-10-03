#include <lemon/render/RenderManager.h>
#include <cassert>

using namespace lemon::render;

static RenderManager* gInstance;

RenderManager::RenderManager() : pDevice{nullptr}
{
    assert(gInstance == nullptr);
    gInstance = this;
}

RenderManager::~RenderManager()
{
    gInstance = nullptr;
}

RenderManager*
RenderManager::get()
{
    return gInstance;
}