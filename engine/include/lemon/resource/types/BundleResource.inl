#pragma once

namespace lemon::res {
    template<class Archive>
    void
    BundleResource::Metadata::serialize(Archive& ar) {
        LEMON_SERIALIZE(ar, common);
    }

    template<typename Archive>
    std::unique_ptr<ResourceMetadataBase>
    BundleResource::loadMetadata(Archive& ar) {
        std::unique_ptr<BundleResource::Metadata> meta = std::make_unique<BundleResource::Metadata>();
        LEMON_SERIALIZE_NVP(ar, "bundle", *meta);
        return meta;
    }

    template<typename Archive>
    void
    BundleResource::saveMetadata(Archive& ar, const ResourceMetadata& data) {
        auto* meta = data.get<BundleResource::Metadata>();
        LEMON_SERIALIZE_NVP(ar, "bundle", *meta);
    }
} // namespace lemon::res