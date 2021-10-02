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
    MaterialResource::Metadata::serialize(TArchive& ar)
    {
        ResourceMetadataBase::serialize(ar);

        LEMON_SERIALIZE(ar, baseType);
        LEMON_SERIALIZE(ar, basePath);
        LEMON_SERIALIZE_FLAGS(ar, usage);
        LEMON_SERIALIZE(ar, domain);
        LEMON_SERIALIZE(ar, shadingModel);
        LEMON_SERIALIZE(ar, blendMode);
        LEMON_SERIALIZE(ar, definitions);
        LEMON_SERIALIZE(ar, samplers);
        LEMON_SERIALIZE(ar, textures);
        LEMON_SERIALIZE(ar, uniforms);

        if (isLoading(ar)) {
            if (baseType == MaterialBaseType::Material) {
                addReference<MaterialResource>(basePath);
            }

            for (auto& kv : textures) {
                addReference<TextureResource>(kv.second);
            }
        }
    }
} // namespace lemon::res