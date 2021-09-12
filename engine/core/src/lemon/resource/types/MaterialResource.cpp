#include <lemon/resource/types/MaterialResource.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/utils/utils.h>

using namespace lemon::res;
using namespace lemon::utils;

MaterialResource::MaterialResource() {
    tprint("MaterialResource::MaterialResource()");
}

MaterialResource::~MaterialResource() {
    tprint("MaterialResource::~MaterialResource()");
}

bool MaterialResource::init(ResourceMetadata& meta) {
    auto* pMetadata = meta.get<Metadata>();
    tprint("MaterialResource::Metadata ptr: ", (uintptr_t)pMetadata);

    return true;
}