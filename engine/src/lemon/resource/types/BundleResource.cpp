#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/BundleResource.h>
#include <lemon/utils/utils.h>

using namespace lemon::res;
using namespace lemon::utils;

VoidTask<ResourceLoadingError>
loadBundle(BundleResource& mat, ResourceMetadata& meta) {
    co_return std::nullopt;
}

BundleResource::BundleResource() {
    lemon::utils::log("BundleResource::BundleResource()");
}

BundleResource::~BundleResource() {
    lemon::utils::log("BundleResource::~BundleResource()");
}

VoidTask<ResourceLoadingError>
BundleResource::load(ResourceMetadata& meta) {
    auto* pMetadata = meta.get<Metadata>();
    lemon::utils::log("BundleResource::Metadata ptr: ", (uintptr_t)pMetadata, " fullPath: ", meta.fullPath,
                      " name: ", meta.name);
    return loadBundle(*this, meta);
}