#include <iostream>

#include <lemon/shared.h>
#include <lemon/shared/filesystem.h>

#include <lemon/resources.h>

#include <dawn/webgpu_cpp.h>

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
saveMetadata(std::unique_ptr<typename T::Metadata>&& data, std::filesystem::path& path)
{
    static std::filesystem::path basePath = R"(C:\git\lemon\resources\)";

    auto fullPath = basePath;
    fullPath /= path;
    fullPath += ".meta";

    std::ofstream os(fullPath.c_str(), std::ios::binary);
    cereal::YAMLOutputArchive ar(os);
    ResourceMetadataDescriptor desc{
        .type = ResourceManager::getClassID<T>(), .data = std::move(data), .fullPath = "", .name = ""};
    ResourceMetadata md(std::move(desc));
    T::saveMetadata(ar, md);
}

void
createMetadata()
{
    using namespace std::filesystem;

    std::vector<std::string> dirs = {"a", "b", "c"};
    std::vector<std::string> bundles = {"RB_A", "RB_B", "RB_C"};
    std::vector<std::string> materials = {"M_A", "M_B", "M_C"};
    std::vector<std::string> textures = {"T_A.png", "T_B.png", "T_C.png"};
    std::vector<std::string> textureTypes = {"albedo", "normal", "roughness"};

    for (int i = 0; i < 3; i++) {
        path p1 = dirs[i];

        auto bun = createBundle();

        for (int j = 0; j < 3; j++) {
            path p2 = p1;
            p2 /= dirs[j];

            auto mat = createMaterial();
            mat->baseType = MaterialResource::BaseType::Shader;
            mat->basePath = "internal\\shaders\\BaseSurfacePBR.wgsl";
            mat->domain.usage = MaterialResource::Usage::StaticMesh | MaterialResource::Usage::SkeletalMesh;
            mat->domain.type = MaterialResource::Domain::Surface;
            mat->domain.shadingModel = MaterialResource::ShadingModel::Lit;
            mat->domain.blendMode = MaterialResource::BlendMode::Opaque;
            mat->samplers.insert({"mySampler1", lemon::res::MaterialResource::SamplerDescriptor()});
            mat->samplers.insert({"mySampler2", lemon::res::MaterialResource::SamplerDescriptor()});
            mat->definitions.insert({"SCROLL_SPEED_U", 0.5f});
            mat->definitions.insert({"SCROLL_SPEED_V", 1.0f});
            mat->definitions.insert({"ENABLE_ARBITRARY_FLAG", true});

            using UniformValue = lemon::res::MaterialResource::UniformValue;

            {
                mat->uniforms.insert({"lemonData.lemonMat", UniformValue{glm::f32mat4x4{}}});
            }

            {
                mat->uniforms.insert(
                    {"lemonData.lemonVecData", UniformValue{glm::f32vec4{1.5f, 2.5f, 3.5f, 4.5f}}});
            }

            {
                mat->uniforms.insert({"lemonData.lemonArr", UniformValue{glm::i32vec4{1, 2, 3, 4}}});
            }

            for (int k = 0; k < 3; k++) {
                path texPath = p2;
                texPath /= textures[k];

                auto tex = createTexture();
                tex->decoder = TextureResource::Decoder::PNG;
                tex->inputChannels = texture::InputColorChannels::RGBA;
                tex->inputChannelDepth = 8;
                tex->GPUFormat = wgpu::TextureFormat::RGBA8UnormSrgb;

                saveMetadata<TextureResource>(std::move(tex), texPath);

                mat->textures.insert({textureTypes[k], texPath.string()});
            }

            path matPath = p1;
            matPath /= materials[j];

            bun->common.references.push_back(RawResourceReference{
                .location = matPath.string(),
                .type = ResourceManager::getClassID<MaterialResource>(),
            });

            saveMetadata<MaterialResource>(std::move(mat), matPath);
        }

        path bunPath = bundles[i];

        saveMetadata<BundleResource>(std::move(bun), bunPath);
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
        path bunPath = "RB_ABC";
        saveMetadata<BundleResource>(std::move(bun), bunPath);
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
        path bunPath = "RB_AB";
        saveMetadata<BundleResource>(std::move(bun), bunPath);
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
        path bunPath = "RB_BC";
        saveMetadata<BundleResource>(std::move(bun), bunPath);
    }

    {
        auto tex = createTexture();
        tex->decoder = TextureResource::Decoder::PNG;
        tex->inputChannels = texture::InputColorChannels::RGBA;
        tex->inputChannelDepth = 8;
        tex->GPUFormat = wgpu::TextureFormat::RGBA8UnormSrgb;

        path texPath = "misc\\T_Basketball_D.png";

        saveMetadata<TextureResource>(std::move(tex), texPath);

        auto mat = createMaterial();
        mat->baseType = MaterialResource::BaseType::Shader;
        mat->basePath = "internal\\shaders\\BaseSurfacePBR.wgsl";
        mat->domain.usage = MaterialResource::Usage::StaticMesh | MaterialResource::Usage::SkeletalMesh;
        mat->domain.type = MaterialResource::Domain::Surface;
        mat->domain.shadingModel = MaterialResource::ShadingModel::Lit;
        mat->domain.blendMode = MaterialResource::BlendMode::Opaque;
        mat->samplers.insert({"surfaceSampler", lemon::res::MaterialResource::SamplerDescriptor()});
        mat->textures.insert({"surfaceTexture", texPath.string()});

        path matPath = "misc\\M_Basketball";

        saveMetadata<MaterialResource>(std::move(mat), matPath);
    }

    {
        auto model = createModel();
        path outPath = R"(ozz-sample\MannequinSkeleton.lem)";
        saveMetadata<ModelResource>(std::move(model), outPath);
    }
}