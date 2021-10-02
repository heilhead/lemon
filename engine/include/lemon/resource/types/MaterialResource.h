#pragma once

#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/TextureResource.h>
#include <lemon/resource/types/material/MaterialComposer.h>
#include <lemon/render/ShaderProgram.h>
#include <lemon/serialization.h>
#include <lemon/serialization/glm.h>
#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>
#include <dawn/webgpu_cpp.h>

namespace lemon::res {
    using namespace lemon::res::material;

    class MaterialResource : public ResourceInstance {
    public:
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

        enum class MaterialBaseType { Shader, Material };
        enum class MaterialUsage { Unknown = 0, StaticMesh = 1 << 0, SkeletalMesh = 1 << 1 };
        enum class MaterialDomain { Surface, PostProcess, UserInterface };
        enum class MaterialShadingModel { Lit, Unlit };
        enum class MaterialBlendMode { Opaque, Masked, Translucent };

        // Note: New types can only be added at the back of the type list of these variants,
        // otherwise serialization will break.
        using MaterialUniformValue =
            std::variant<int32_t, glm::i32vec2, glm::i32vec4, uint32_t, glm::u32vec2, glm::u32vec4, float,
                         glm::f32vec2, glm::f32vec4, glm::f32mat4x4>;
        using MaterialDefinitionValue = std::variant<int32_t, uint32_t, float, bool, std::string>;

        /////////////////////////////////////////////////////////////////////////////////////
        // BEGIN Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

        struct Metadata : ResourceMetadataBase {
            MaterialBaseType baseType;
            std::string basePath;
            MaterialUsage usage;
            MaterialDomain domain;
            MaterialShadingModel shadingModel;
            MaterialBlendMode blendMode;
            std::unordered_map<std::string, MaterialDefinitionValue> definitions;
            std::unordered_map<std::string, SamplerDescriptor> samplers;
            std::unordered_map<std::string, std::string> textures;
            std::unordered_map<std::string, MaterialUniformValue> uniforms;

            template<class TArchive>
            void
            serialize(TArchive& ar);
        };

        LEMON_RESOURCE_TRAITS(MaterialResource);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

    private:
        Metadata metadata;
        std::optional<Blueprint> blueprint;
        BlueprintConfiguration config;

    public:
        MaterialResource();
        ~MaterialResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata&& meta) override;

        const class render::ShaderProgram*
        getShader(const BlueprintConfiguration& pipelineConfig);
    };
} // namespace lemon::res

#include <lemon/resource/types/MaterialResource.inl>
