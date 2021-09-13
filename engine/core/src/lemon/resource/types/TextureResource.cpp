#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/TextureResource.h>
#include <lemon/utils/utils.h>

using namespace lemon::res;
using namespace lemon::utils;

VoidTask<ResourceLoadingError>
loadTexture(TextureResource& mat, ResourceMetadata& meta) {
    co_return std::nullopt;
}

TextureResource::TextureResource() {
    tprint("TextureResource::TextureResource()");
}

TextureResource::~TextureResource() {
    tprint("TextureResource::~TextureResource()");
}

VoidTask<ResourceLoadingError>
TextureResource::load(ResourceMetadata& meta) {
    auto* pMetadata = meta.get<Metadata>();
    tprint("TextureResource::Metadata ptr: ", (uintptr_t)pMetadata, " fullPath: ", meta.fullPath,
           " name: ", meta.name);
    return loadTexture(*this, meta);
}