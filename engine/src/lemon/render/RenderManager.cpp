#include <lemon/render/RenderManager.h>
#include <lemon/shared/assert.h>

using namespace lemon::render;

static RenderManager* gInstance;

RenderManager::RenderManager() : pDevice{nullptr}
{
    LEMON_ASSERT(gInstance == nullptr);
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