#pragma once

#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>

namespace lemon::res {
    class BundleResource : public ResourceInstance {
    public:
        /////////////////////////////////////////////////////////////////////////////////////
        // BEGIN Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

        struct Metadata : ResourceMetadataBase {
            template<class TArchive>
            void
            serialize(TArchive& ar) {
                ResourceMetadataBase::serialize(ar);
            }
        };

        LEMON_RESOURCE_TRAITS(BundleResource);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

    public:
        BundleResource();
        ~BundleResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata& meta) override;
    };
} // namespace lemon::res
