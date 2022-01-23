#pragma once

#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/TextureResource.h>
#include <lemon/resource/types/material/MaterialComposer.h>
#include <lemon/render/material/common.h>
#include <lemon/render/material/ShaderProgram.h>
#include <lemon/render/material/MaterialConfiguration.h>
#include <lemon/serialization.h>
#include <lemon/serialization/glm.h>
#include <lemon/scheduler.h>

namespace lemon::res {
    namespace material {
        struct SamplerDescriptor {
            wgpu::AddressMode addressModeU = wgpu::AddressMode::ClampToEdge;
            wgpu::AddressMode addressModeV = wgpu::AddressMode::ClampToEdge;
            wgpu::AddressMode addressModeW = wgpu::AddressMode::ClampToEdge;
            wgpu::FilterMode magFilter = wgpu::FilterMode::Nearest;
            wgpu::FilterMode minFilter = wgpu::FilterMode::Nearest;
            wgpu::FilterMode mipmapFilter = wgpu::FilterMode::Nearest;
            float lodMinClamp = 0.0f;
            float lodMaxClamp = 1000.0f;
            wgpu::CompareFunction compare = wgpu::CompareFunction::Undefined;
            uint16_t maxAnisotropy = 1;

            template<class TArchive>
            void
            serialize(TArchive& ar);
        };
    } // namespace material

    class MaterialResource : public ResourceInstance {
    public:
        // Note: New types can only be added at the back of the type list of these variants,
        // otherwise serialization will break.
        using UniformValue = std::variant<int32_t, glm::i32vec2, glm::i32vec4, uint32_t, glm::u32vec2,
                                          glm::u32vec4, float, glm::f32vec2, glm::f32vec4, glm::f32mat4x4>;

        template<typename T, size_t Size>
        using ResourceList = folly::small_vector<std::pair<StringID, T>, Size>;
        using SamplerDescriptorList = ResourceList<material::SamplerDescriptor, 4>;
        using TextureLocationList = ResourceList<ResourceLocation, 8>;
        using UniformValueList = ResourceList<UniformValue, 8>;

        enum class BaseType { Shader, Material };
        enum class Usage { Unknown = 1 << 0, StaticMesh = 1 << 1, SkeletalMesh = 1 << 2 };
        enum class Domain { Surface, PostProcess, UserInterface, Dynamic };
        enum class ShadingModel { Lit, Unlit };
        enum class BlendMode { Opaque, Masked, Translucent };

        struct DomainDescriptor {
            Domain type = Domain::Surface;
            Usage usage = Usage::StaticMesh;
            ShadingModel shadingModel = ShadingModel::Lit;
            BlendMode blendMode = BlendMode::Opaque;

            template<class TArchive>
            void
            serialize(TArchive& ar);
        };

        /////////////////////////////////////////////////////////////////////////////////////
        // BEGIN Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

        struct Metadata : ResourceMetadataBase {
            template<typename T>
            using ResourceList = std::vector<std::pair<std::string, T>>;

            BaseType baseType = BaseType::Shader;
            std::string basePath{};

            DomainDescriptor domain{};

            ResourceList<render::MaterialConfiguration::Value> definitions;
            ResourceList<material::SamplerDescriptor> samplers;
            ResourceList<std::string> textures;
            ResourceList<UniformValue> uniforms;

            template<class TArchive>
            void
            serialize(TArchive& ar);
        };

        LEMON_RESOURCE_TRAITS(MaterialResource);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

    private:
        DomainDescriptor domain;
        std::optional<material::MaterialBlueprint> blueprint;
        render::MaterialConfiguration config;
        SamplerDescriptorList samplers;
        TextureLocationList textures;
        UniformValueList uniforms;

    public:
        MaterialResource();
        ~MaterialResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata&& meta) override;

        inline const std::optional<material::MaterialBlueprint>&
        getBlueprint() const
        {
            return blueprint;
        }

        inline const render::MaterialConfiguration&
        getConfig() const
        {
            return config;
        }

        inline const DomainDescriptor&
        getDomainDescriptor() const
        {
            return domain;
        }

        inline const SamplerDescriptorList&
        getSamplerDescriptors() const
        {
            return samplers;
        }

        inline const TextureLocationList&
        getTextureLocations() const
        {
            return textures;
        }

        inline const UniformValueList&
        getUniformValues() const
        {
            return uniforms;
        }

        static Task<material::MaterialBlueprint, ResourceLoadingError>
        loadShaderBlueprint(const std::string& bplPath);
    };
} // namespace lemon::res

#include <lemon/resource/types/MaterialResource.inl>
