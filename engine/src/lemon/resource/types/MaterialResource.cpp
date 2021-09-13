#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/MaterialResource.h>
#include <lemon/utils/utils.h>

using namespace lemon::res;
using namespace lemon::utils;

VoidTask<ResourceLoadingError>
loadMaterial(MaterialResource& mat, ResourceMetadata& meta) {
    co_return std::nullopt;
}

MaterialResource::MaterialResource() {
    tprint("MaterialResource::MaterialResource()");
}

MaterialResource::~MaterialResource() {
    tprint("MaterialResource::~MaterialResource()");
}

VoidTask<ResourceLoadingError>
MaterialResource::load(ResourceMetadata& meta) {
    auto* pMetadata = meta.get<Metadata>();
    tprint("MaterialResource::Metadata ptr: ", (uintptr_t)pMetadata, " fullPath: ", meta.fullPath,
           " name: ", meta.name);
    return loadMaterial(*this, meta);
}