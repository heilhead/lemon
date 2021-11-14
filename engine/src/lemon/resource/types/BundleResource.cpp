#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/BundleResource.h>

using namespace lemon::res;
using namespace lemon::utils;

VoidTask<ResourceLoadingError>
loadBundle(BundleResource& mat, ResourceMetadata& meta)
{
    co_return std::nullopt;
}

BundleResource::BundleResource() {}

BundleResource::~BundleResource() {}

VoidTask<ResourceLoadingError>
BundleResource::load(ResourceMetadata&& meta)
{
    auto* pMetadata = meta.get<Metadata>();
    logger::log("BundleResource::Metadata ptr: ", (uintptr_t)pMetadata, " fullPath: ", meta.fullPath,
                " name: ", meta.name);
    return loadBundle(*this, meta);
}
