#pragma once

namespace lemon::res {
    template<class Archive>
    void
    TextureResource::Metadata::serialize(Archive& ar) {
        ar(CEREAL_NVP(format));
    }

    template<typename Archive>
    std::unique_ptr<ResourceMetadataBase>
    TextureResource::loadMetadata(Archive& ar) {
        std::unique_ptr<TextureResource::Metadata> meta = std::make_unique<TextureResource::Metadata>();
        ar(cereal::make_nvp("texture", *meta));
        return meta;
    }

    template<typename Archive>
    void
    TextureResource::saveMetadata(Archive& ar, const ResourceMetadata& data) {
        auto* meta = data.get<TextureResource::Metadata>();
        ar(cereal::make_nvp("texture", *meta));
    }
} // namespace lemon::res