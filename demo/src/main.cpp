#include <iostream>
//#include <variant>
//#include <vector>
//#include <cassert>
//#include <chrono>
//#include <cstdint>
#include <mutex>
//#include <string>

#include <lemon/scheduler.h>
//#include <folly/experimental/coro/BlockingWait.h>
//#include <folly/experimental/coro/Collect.h>

#include <lemon/shared.h>
#include <lemon/shared/filesystem.h>
#include <lemon/resource/ResourceManager.h>

#include <cereal/archives/xml.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <fstream>

std::mutex mut;

template<typename ...Args>
void print(Args&& ...args) {
    std::lock_guard lg(mut);
    (std::cout << ... << args) << std::endl;
}

//void gameLoopPseudocode() {
//    while (true) {
//        gFrameManager.swap();
//
//        auto gameStageFuture = gScheduler.schedule([] {
//            co_await updateInput();
//            co_await updatePhysics();
//            co_await updateGameObjects();
//            co_await updateAI();
//            co_await updateAnimationData();
//            co_await prepareRenderData();
//        });
//
//        auto renderStageFuture = gScheduler.schedule([] {
//            co_await updateTransformMatrices();
//            co_await calculateCulling();
//            co_await prepareCommandLists();
//
//            dispatchRender();
//
//            co_await presentFrame();
//        });
//
//        gFrameManager.blockGameLoop(
//            gameStageFuture,
//            renderStageFuture,
//        );
//    }
//}

using namespace lemon::scheduler;
using namespace lemon::res;

enum class ResourceType {
    Unknown,
    Level,
    Model,
    Material,
    Texture,
    Mesh,
};

struct RawResourceReference {
    std::string location;
    ResourceType type;

    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(location));
        ar(CEREAL_NVP(type));
    }
};

struct CommonResourceMetadata {
    std::vector<RawResourceReference> references;

    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(references));
    }
};

struct ResourceMetadataBase {
    virtual ~ResourceMetadataBase() {}
};

struct MaterialResourceMetadata : ResourceMetadataBase {
    CommonResourceMetadata common;
    std::unordered_map<std::string, int> shaderConfig;
    std::unordered_map<std::string, std::string> textures;

    template<class Archive>
    void serialize(Archive& ar) {
        ar(CEREAL_NVP(common));
        ar(CEREAL_NVP(shaderConfig));
        ar(CEREAL_NVP(textures));
    }
};

struct ResourceMetadata {
    ResourceType type;
    std::unique_ptr<ResourceMetadataBase> data;

    ResourceMetadata(ResourceType inType, std::unique_ptr<ResourceMetadataBase>&& inData) {
        type = inType;
        data = std::move(inData);
    }

    template<typename T>
    [[nodiscard]] T* get() const {
        return dynamic_cast<T*>(data.get());
    }
};

enum class ResourceLoadingError {
    Unknown,
};

template<typename Loader>
tl::expected<ResourceMetadata, ResourceLoadingError>
loadMetadata(std::string& path) {
    auto location = ResourceLocation(path);
    auto fullPath = ResourceManager::get()->locateFile(location);
    fullPath += ".meta";

    return lemon::io::readTextFile(fullPath)
        .map_error([](auto&& err) {
            return ResourceLoadingError::Unknown;
        })
        .map([](auto&& str) {
            std::istringstream is(str);
            cereal::XMLInputArchive archive(is);
            ResourceMetadata meta(Loader::getType(), Loader::loadMetadata(archive));
            return meta;
        });
}

struct MaterialLoader {
    static constexpr ResourceType getType() {
        return ResourceType::Material;
    }

    template<typename Archive>
    static std::unique_ptr<ResourceMetadataBase> loadMetadata(Archive& ar) {
        std::unique_ptr<MaterialResourceMetadata> data = std::make_unique<MaterialResourceMetadata>();
        ar(cereal::make_nvp("material", *data));
        return data;
    }

    template<typename Archive>
    static void saveMetadata(Archive& ar, const ResourceMetadata& data) {
        auto* matData = data.get<MaterialResourceMetadata>();
        ar(cereal::make_nvp("material", *matData));
    }
};

int main(int argc, char* argv[]) {
    std::unique_ptr<lemon::res::ResourceManager> resMan = std::make_unique<lemon::res::ResourceManager>(
        R"(C:\git\lemon\resources)");

    std::string path = "basketball.mat";
    auto meta = loadMetadata<MaterialLoader>(path);

    if (meta) {
        auto* matData = (*meta).get<MaterialResourceMetadata>();
        print("load metadata: success");
    } else {
        print("load metadata: error");
    }

//    {
//        std::ofstream os("C:\\git\\lemon\\resources\\basketball.mat.meta", std::ios::binary);
//        cereal::XMLOutputArchive archive(os);
//
//        auto material = std::make_unique<MaterialResourceMetadata>();
//        material->common.references.push_back(RawResourceReference {
//            .location = "basketball.png",
//            .type = ResourceType::Texture
//        });
//        material->shaderConfig.insert({ "ENABLE_SKINNING", 0 });
//        material->textures.insert({ "albedo", "basketball.png" });
//        ResourceMetadata meta(MaterialLoader::getType(), std::move(material));
//        MaterialLoader::saveMetadata(archive, meta);
//    }

//    std::unique_ptr<Scheduler> sched = std::make_unique<Scheduler>();
//
//    auto result = folly::coro::blockingWait(
//        IOTask(readTextFile("C:\\git\\lemon\\engine\\core\\include\\lemon\\engine.h"))
//    );
//
//    if (result) {
//        std::cout << "result ok: " << *result << std::endl;
//    } else {
//        std::cout << "result error: " << (int)result.error() << std::endl;
//    }

    return 0;
}
