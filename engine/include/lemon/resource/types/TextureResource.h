#pragma once

#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::res {

    class TextureResource : public ResourceInstance {
    public:
        /////////////////////////////////////////////////////////////////////////////////////
        // BEGIN Resource interface
        /////////////////////////////////////////////////////////////////////////////////////

        static constexpr ResourceType
        getType() {
            return ResourceType::Texture;
        }

        struct Metadata : ResourceMetadataBase {
            wgpu::TextureFormat format;

            template<class Archive>
            void
            serialize(Archive& ar);
        };

        template<typename Archive>
        static std::unique_ptr<ResourceMetadataBase>
        loadMetadata(Archive& ar);

        template<typename Archive>
        static void
        saveMetadata(Archive& ar, const ResourceMetadata& data);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource interface
        /////////////////////////////////////////////////////////////////////////////////////

    public:
        TextureResource();
        ~TextureResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata& meta) override;
    };
} // namespace lemon::res

#include <lemon/resource/types/TextureResource.inl>
