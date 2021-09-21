#include <iostream>

#include <lemon/shared.h>
#include <lemon/shared/filesystem.h>

#include <lemon/resources.h>

#include <dawn/webgpu_cpp.h>

using namespace lemon::scheduler;
using namespace lemon::res;

auto
createTexture() {
    return std::make_unique<TextureResource::Metadata>();
}

auto
createMaterial() {
    return std::make_unique<MaterialResource::Metadata>();
}

auto
createBundle() {
    return std::make_unique<BundleResource::Metadata>();
}

auto
createModel() {
    return std::make_unique<ModelResource::Metadata>();
}

void
addDependencies(ResourceMetadataBase* data, std::initializer_list<RawResourceReference> refs) {
    data->common.references = refs;
}

template<typename T>
void
saveMetadata(std::unique_ptr<typename T::Metadata>&& data, std::filesystem::path& path) {
    static std::filesystem::path basePath = R"(C:\git\lemon\resources\)";

    auto fullPath = basePath;
    fullPath /= path;
    fullPath += ".meta";

    std::ofstream os(fullPath.c_str(), std::ios::binary);
    cereal::XMLOutputArchive ar(os);
    ResourceMetadataDescriptor desc{
        .type = ResourceManager::getClassID<T>(), .data = std::move(data), .fullPath = "", .name = ""};
    ResourceMetadata md(std::move(desc));
    T::saveMetadata(ar, md);
}

void
createMetadata() {
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
            mat->shaderConfig.insert({"ENABLE_SKINNING", 0});

            for (int k = 0; k < 3; k++) {
                path texPath = p2;
                texPath /= textures[k];

                auto tex = createTexture();
                tex->decoder = TextureResource::Decoder::PNG;
                tex->format = wgpu::TextureFormat::RGBA8UnormSrgb;
                tex->width = 512;
                tex->height = 512;

                saveMetadata<TextureResource>(std::move(tex), texPath);

                mat->common.references.push_back(RawResourceReference{
                    .location = texPath.string(),
                    .type = ResourceManager::getClassID<TextureResource>(),
                });
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
        auto model = createModel();
        path outPath = R"(ozz-sample\MannequinSkeleton.lem)";
        saveMetadata<ModelResource>(std::move(model), outPath);
    }
}