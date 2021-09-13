#pragma once

namespace lemon::res {
    template<class Archive>
    void
    BundleResource::Metadata::serialize(Archive& ar) {
        ar(CEREAL_NVP(common));
    }

    template<typename Archive>
    std::unique_ptr<ResourceMetadataBase>
    BundleResource::loadMetadata(Archive& ar) {
        std::unique_ptr<BundleResource::Metadata> meta = std::make_unique<BundleResource::Metadata>();
        ar(cereal::make_nvp("bundle", *meta));
        return meta;
    }

    template<typename Archive>
    void
    BundleResource::saveMetadata(Archive& ar, const ResourceMetadata& data) {
        auto* meta = data.get<BundleResource::Metadata>();
        ar(cereal::make_nvp("bundle", *meta));
    }
} // namespace lemon::res