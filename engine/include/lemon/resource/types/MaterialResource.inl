#pragma once

namespace lemon::res {
    template<class TArchive>
    void
    MaterialResource::SamplerDescriptor::serialize(TArchive& ar)
    {
        LEMON_SERIALIZE(ar, addressModeU);
        LEMON_SERIALIZE(ar, addressModeV);
        LEMON_SERIALIZE(ar, addressModeW);
        LEMON_SERIALIZE(ar, magFilter);
        LEMON_SERIALIZE(ar, minFilter);
        LEMON_SERIALIZE(ar, mipmapFilter);
        LEMON_SERIALIZE(ar, lodMinClamp);
        LEMON_SERIALIZE(ar, lodMaxClamp);
        LEMON_SERIALIZE(ar, compare);
        LEMON_SERIALIZE(ar, maxAnisotropy);
    }

    template<class TArchive>
    void
    MaterialResource::DomainDescriptor::serialize(TArchive& ar)
    {
        LEMON_SERIALIZE(ar, type);
        LEMON_SERIALIZE_FLAGS(ar, usage);
        LEMON_SERIALIZE(ar, shadingModel);
        LEMON_SERIALIZE(ar, blendMode);
    }

    template<class TArchive>
    void
    MaterialResource::Metadata::serialize(TArchive& ar)
    {
        ResourceMetadataBase::serialize(ar);

        LEMON_SERIALIZE(ar, baseType);
        LEMON_SERIALIZE(ar, basePath);
        LEMON_SERIALIZE(ar, domain);
        LEMON_SERIALIZE(ar, definitions);
        LEMON_SERIALIZE(ar, samplers);
        LEMON_SERIALIZE(ar, textures);
        LEMON_SERIALIZE(ar, uniforms);

        if (isLoading(ar)) {
            if (baseType == BaseType::Material) {
                addReference<MaterialResource>(basePath);
            }

            for (auto& [name, filePath] : textures) {
                addReference<TextureResource>(filePath);
            }
        }
    }
} // namespace lemon::res

template<>
struct folly::hasher<lemon::res::MaterialResource::SamplerDescriptor> {
    using folly_is_avalanching = std::true_type;

    size_t
    operator()(const lemon::res::MaterialResource::SamplerDescriptor& data) const
    {
        return lemon::hash(data.addressModeU, data.addressModeV, data.addressModeW, data.magFilter,
                           data.minFilter, data.mipmapFilter, data.lodMinClamp, data.lodMaxClamp,
                           data.compare, data.maxAnisotropy);
    }
};