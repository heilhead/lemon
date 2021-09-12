#pragma once

namespace lemon::res {
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