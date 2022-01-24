#include <lemon/resource/types/ModelResource.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/ResourceManager.h>

using namespace lemon::res;
using namespace lemon::utils;

enum class ModelDataLoadingError { Unknown, StreamError };

Task<std::unique_ptr<model::LemonModel>, ModelDataLoadingError>
loadData(std::filesystem::path& file)
{
    if (std::ifstream stream{file, std::ios::binary}) {
        cereal::BinaryInputArchive archive(stream);
        auto data = std::make_unique<model::LemonModel>();
        archive(*data);
        co_return std::move(data);
    } else {
        co_return tl::make_unexpected(ModelDataLoadingError::StreamError);
    }
}

ModelResource::ModelResource() {}

ModelResource::~ModelResource() {}

VoidTask<ResourceLoadingError>
ModelResource::load(ResourceMetadata&& meta)
{
    OPTICK_EVENT();

    auto* pMetadata = meta.get<Metadata>();

    logger::log("ModelResource::Metadata ptr: ", (uintptr_t)pMetadata, " fullPath: ", meta.fullPath,
                " name: ", meta.name);
    auto dataFile = ResourceManager::get()->resolvePath(meta.name);
    auto result = co_await runIOTask(loadData(dataFile));
    if (result) {
        data = std::move(*result);

        for (auto& node : data->nodes) {
            ResourceObjectHandle handle(node.name);
            createObject<Model>(handle, data.get(), &node);

            logger::log("LemonModel object created: ", getObject<Model>(handle)->getName());
        }

        logger::log("LemonModel nodes: ", data->nodes.size(), " meshes: ", data->meshes.size());
    } else {
        logger::err("LemonModel load error: ", EnumTraits::name(result.error()));
    }

    co_return {};
}
