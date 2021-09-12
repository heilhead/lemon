#pragma once

#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/utils/utils.h>

namespace lemon::res {
    class MaterialResource : public ResourceInstance {
    public:
        /////////////////////////////////////////////////////////////////////////////////////
        // BEGIN Resource interface
        /////////////////////////////////////////////////////////////////////////////////////

        static constexpr ResourceType getType() {
            return ResourceType::Material;
        }

        struct Metadata : ResourceMetadataBase {
            CommonResourceMetadata common;
            std::unordered_map<std::string, int> shaderConfig;
            std::unordered_map<std::string, std::string> textures;

            template<class Archive>
            void serialize(Archive& ar);
        };

        template<typename Archive>
        static std::unique_ptr<ResourceMetadataBase> loadMetadata(Archive& ar);

        template<typename Archive>
        static void saveMetadata(Archive& ar, const ResourceMetadata& data);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource interface
        /////////////////////////////////////////////////////////////////////////////////////

    public:
        MaterialResource();
        ~MaterialResource() override;
    };

    template<class Archive>
    void MaterialResource::Metadata::serialize(Archive& ar) {
        ar(CEREAL_NVP(common));
        ar(CEREAL_NVP(shaderConfig));
        ar(CEREAL_NVP(textures));
    }

    template<typename Archive>
    std::unique_ptr<ResourceMetadataBase> MaterialResource::loadMetadata(Archive& ar) {
        std::unique_ptr<MaterialResource::Metadata> meta = std::make_unique<MaterialResource::Metadata>();
        ar(cereal::make_nvp("material", *meta));
        return meta;
    }

    template<typename Archive>
    void MaterialResource::saveMetadata(Archive& ar, const ResourceMetadata& data) {
        auto* meta = data.get<MaterialResource::Metadata>();
        ar(cereal::make_nvp("material", *meta));
    }
}
