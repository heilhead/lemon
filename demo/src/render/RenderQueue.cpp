#include "RenderQueue.h"

using namespace lemon;
using namespace lemon::render;

RenderQueue::RenderQueue() : meshes{kMeshStoreCapacity}, lights{kLightStoreCapacity} {}
