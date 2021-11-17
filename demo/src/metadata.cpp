#include "metadata.h"

using namespace lemon::scheduler;
using namespace lemon::res;

auto
createTexture()
{
    return std::make_unique<TextureResource::Metadata>();
}

auto
createMaterial()
{
    return std::make_unique<MaterialResource::Metadata>();
}

auto
createBundle()
{
    return std::make_unique<BundleResource::Metadata>();
}

auto
createModel()
{
    return std::make_unique<ModelResource::Metadata>();
}

void
addDependencies(ResourceMetadataBase* data, std::initializer_list<RawResourceReference> refs)
{
    data->common.references = refs;
}

template<typename T>
void
saveMetadata(std::unique_ptr<typename T::Metadata> data, const std::filesystem::path& path)
{
    static std::filesystem::path basePath = R"(C:\git\lemon\resources\)";

    auto fullPath = basePath / path;
    fullPath += ".meta";

    std::ofstream os(fullPath, std::ios::binary);
    cereal::YAMLOutputArchive ar(os);
    ResourceMetadataDescriptor desc{
        .type = ResourceManager::getClassID<T>(), .data = std::move(data), .fullPath = "", .name = ""};

    T::saveMetadata(ar, ResourceMetadata(std::move(desc)));
}

void
createMetadata()
{
    using namespace std::filesystem;
    using UniformValue = MaterialResource::UniformValue;

    std::vector<std::string> dirs = {"a", "b", "c"};
    std::vector<std::string> bundles = {"RB_A", "RB_B", "RB_C"};
    std::vector<std::string> materials = {"M_A", "M_B", "M_C"};
    std::vector<std::string> textures = {"T_A.png", "T_B.png", "T_C.png"};
    std::vector<std::string> textureTypes = {"albedo", "normal", "roughness"};

    for (int i = 0; i < 3; i++) {
        path p1 = dirs[i];

        auto bun = createBundle();

        for (int j = 0; j < 3; j++) {
            path p2 = p1 / dirs[j];

            auto mat = createMaterial();
            mat->baseType = MaterialResource::BaseType::Shader;
            mat->basePath = "internal\\shaders\\BaseSurface.wgsl";
            mat->domain.usage = MaterialResource::Usage::StaticMesh | MaterialResource::Usage::SkeletalMesh;
            mat->domain.type = MaterialResource::Domain::Surface;
            mat->domain.shadingModel = MaterialResource::ShadingModel::Lit;
            mat->domain.blendMode = MaterialResource::BlendMode::Opaque;
            mat->samplers.emplace_back(
                std::make_pair("surfaceSampler", lemon::res::material::SamplerDescriptor()));
            mat->definitions.emplace_back(std::make_pair("MATERIAL_SCROLL_SPEED_U", 0.5f));
            mat->definitions.emplace_back(std::make_pair("MATERIAL_SCROLL_SPEED_V", 1.0f));
            mat->definitions.emplace_back(std::make_pair("MATERIAL_ENABLE_ARBITRARY_FLAG", true));
            mat->uniforms.emplace_back(
                std::make_pair("packetParams.modelMatrix", UniformValue{glm::f32mat4x4{}}));
            mat->uniforms.emplace_back(
                std::make_pair("materialParams.tint", UniformValue{glm::f32vec4{1.0f, 0.5f, 0.5f, 0.5f}}));

            for (int k = 0; k < 3; k++) {
                path texPath = p2 / textures[k];

                auto tex = createTexture();
                tex->sourceDecoder = TextureResource::Decoder::PNG;
                tex->sourceChannels = texture::InputColorChannels::RGBA;
                tex->sourceChannelDepth = 8;
                tex->GPUFormat = wgpu::TextureFormat::RGBA8UnormSrgb;

                saveMetadata<TextureResource>(std::move(tex), texPath);

                mat->textures.emplace_back(std::make_pair(textureTypes[k], texPath.string()));
            }

            path matPath = p1 / materials[j];

            bun->common.references.push_back(RawResourceReference{
                .location = matPath.string(),
                .type = ResourceManager::getClassID<MaterialResource>(),
            });

            saveMetadata<MaterialResource>(std::move(mat), matPath);
        }

        saveMetadata<BundleResource>(std::move(bun), bundles[i]);
    }

    {
        auto bun = createBundle();
        bun->common.references.push_back(RawResourceReference{
            .location = "RB_AB",
            .type = ResourceManager::getClassID<BundleResource>(),
        });
        bun->common.references.push_back(RawResourceReference{
            .location = "RB_BC",
            .type = ResourceManager::getClassID<BundleResource>(),
        });

        saveMetadata<BundleResource>(std::move(bun), "RB_ABC");
    }

    {
        auto bun = createBundle();
        bun->common.references.push_back(RawResourceReference{
            .location = "RB_A",
            .type = ResourceManager::getClassID<BundleResource>(),
        });
        bun->common.references.push_back(RawResourceReference{
            .location = "RB_B",
            .type = ResourceManager::getClassID<BundleResource>(),
        });

        saveMetadata<BundleResource>(std::move(bun), "RB_AB");
    }

    {
        auto bun = createBundle();
        bun->common.references.push_back(RawResourceReference{
            .location = "RB_B",
            .type = ResourceManager::getClassID<BundleResource>(),
        });
        bun->common.references.push_back(RawResourceReference{
            .location = "RB_C",
            .type = ResourceManager::getClassID<BundleResource>(),
        });

        saveMetadata<BundleResource>(std::move(bun), "RB_BC");
    }

    {
        auto texAO = createTexture();
        texAO->sourceDecoder = TextureResource::Decoder::PNG;
        texAO->sourceChannels = texture::InputColorChannels::RGBA;
        texAO->sourceChannelDepth = 8;
        texAO->GPUFormat = wgpu::TextureFormat::RGBA8Unorm;

        path texAOPath = "misc\\T_Mannequin_AO.png";

        saveMetadata<TextureResource>(std::move(texAO), texAOPath);

        auto texN = createTexture();
        texN->sourceDecoder = TextureResource::Decoder::PNG;
        texN->sourceChannels = texture::InputColorChannels::RGBA;
        texN->sourceChannelDepth = 8;
        texN->GPUFormat = wgpu::TextureFormat::RGBA8Unorm;

        path texNPath = "misc\\T_Mannequin_N.png";

        saveMetadata<TextureResource>(std::move(texN), texNPath);

        {
            auto mat = createMaterial();
            mat->baseType = MaterialResource::BaseType::Shader;
            mat->basePath = "internal\\shaders\\BaseSurface.wgsl";
            mat->domain.usage = MaterialResource::Usage::StaticMesh | MaterialResource::Usage::SkeletalMesh;
            mat->domain.type = MaterialResource::Domain::Surface;
            mat->domain.shadingModel = MaterialResource::ShadingModel::Lit;
            mat->domain.blendMode = MaterialResource::BlendMode::Opaque;
            mat->samplers.emplace_back(
                std::make_pair("surfaceSampler", lemon::res::material::SamplerDescriptor()));
            mat->textures.emplace_back(std::make_pair("tAlbedo", texAOPath.string()));
            mat->textures.emplace_back(std::make_pair("tNormal", texNPath.string()));
            mat->uniforms.emplace_back(
                std::make_pair("packetParams.matModel", UniformValue{glm::f32mat4x4{}}));
            mat->uniforms.emplace_back(
                std::make_pair("materialParams.tint", UniformValue{glm::f32vec4{1.0f, 0.5f, 0.5f, 0.5f}}));

            saveMetadata<MaterialResource>(std::move(mat), "misc\\M_Mannequin");
        }

        {
            auto mat = createMaterial();
            mat->baseType = MaterialResource::BaseType::Material;
            mat->basePath = "misc\\M_Mannequin";
            mat->domain.usage = MaterialResource::Usage::StaticMesh | MaterialResource::Usage::SkeletalMesh;
            mat->domain.type = MaterialResource::Domain::Surface;
            mat->domain.shadingModel = MaterialResource::ShadingModel::Lit;
            mat->domain.blendMode = MaterialResource::BlendMode::Opaque;
            mat->uniforms.emplace_back(
                std::make_pair("materialParams.tint", UniformValue{glm::f32vec4{0.5f, 1.0f, 0.5f, 0.5f}}));

            saveMetadata<MaterialResource>(std::move(mat), "misc\\M_Mannequin2");
        }
    }

    {
        saveMetadata<ModelResource>(createModel(), R"(ozz-sample\MannequinSkeleton.lem)");
    }

    {
        auto tex = createTexture();
        tex->type = TextureResource::Type::RenderTarget;
        tex->GPUFormat = wgpu::TextureFormat::RGBA8Unorm;
        tex->width = 640;
        tex->height = 400;
        tex->mipLevelCount = 6;
        tex->sourceDecoder = TextureResource::Decoder::None;

        path texPath = "internal\\textures\\T_PostProcess_Bloom";

        saveMetadata<TextureResource>(std::move(tex), texPath);

        auto mat = createMaterial();
        mat->baseType = MaterialResource::BaseType::Shader;
        mat->basePath = "internal\\shaders\\BasePostProcess.wgsl";
        mat->domain.type = MaterialResource::Domain::PostProcess;
        mat->domain.usage = MaterialResource::Usage::Unknown;

        auto sBloomSampler = lemon::res::material::SamplerDescriptor();
        sBloomSampler.minFilter = wgpu::FilterMode::Linear;
        sBloomSampler.magFilter = wgpu::FilterMode::Linear;

        mat->samplers.emplace_back(std::make_pair("sBloomSampler", sBloomSampler));
        mat->textures.emplace_back(std::make_pair("tBloom", texPath.string()));

        path matPath = "internal\\materials\\M_PostProcess";

        saveMetadata<MaterialResource>(std::move(mat), matPath);
    }
}
