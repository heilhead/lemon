#include <iostream>
//#include <variant>
//#include <vector>
//#include <cassert>
//#include <chrono>
//#include <cstdint>
#include <mutex>
//#include <string>

#include <lemon/scheduler.h>
#include <folly/experimental/coro/BlockingWait.h>
#include <folly/experimental/coro/Collect.h>

#include <lemon/shared.h>
#include <lemon/shared/filesystem.h>
#include <lemon/resource/ResourceManager.h>

#include <cereal/archives/xml.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
//#include <fstream>

#include <folly/AtomicHashMap.h>
#include <folly/PackedSyncPtr.h>

std::mutex mut;

template<typename ...Args>
void print(Args&& ...args) {
    std::lock_guard lg(mut);
    (std::cout << ... << args) << std::endl;
}

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

template<typename TResource>
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
            ResourceMetadata meta(TResource::getType(), TResource::loadMetadata(archive));
            return meta;
        });
}

class ResourceInstance {
public:
    virtual ~ResourceInstance() {}
};

class MaterialResource : public ResourceInstance {
public:
    static constexpr ResourceType getType() {
        return ResourceType::Material;
    }

    struct Metadata : ResourceMetadataBase {
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

    template<typename Archive>
    static std::unique_ptr<ResourceMetadataBase> loadMetadata(Archive& ar) {
        std::unique_ptr<Metadata> data = std::make_unique<Metadata>();
        ar(cereal::make_nvp("material", *data));
        return data;
    }

    template<typename Archive>
    static void saveMetadata(Archive& ar, const ResourceMetadata& data) {
        auto* matData = data.get<Metadata>();
        ar(cereal::make_nvp("material", *matData));
    }

    MaterialResource() {
        print("MaterialResource constructor");
    }

    ~MaterialResource() {
        print("MaterialResource destructor");
    }
};

struct ResourceContract {
public:
    ResourceContract()
        :contract {
        std::move(folly::makePromiseContract<void*>(Scheduler::get()->getCPUExecutor()->weakRef()))
    } {
        print("ResourceContract constructor");
    }

    ~ResourceContract() {
        // Assume that if this destructor is called, the resource is either fully loaded, or failed to load. Either way,
        // we can destroy the data we have.
        if (contract.first.isFulfilled()) {
            auto* data = reinterpret_cast<ResourceInstance*>(contract.second.value());
            delete data;
        }
        print("ResourceContract destructor");
    }

private:
    std::pair<folly::Promise<void*>, folly::SemiFuture<void*>> contract;

public:
    template<typename T = void>
    folly::Promise<T*>* getPromise() {
        return &contract.first;
    }

    template<typename T = void>
    folly::SemiFuture<T*>* getFuture() {
        return &contract.second;
    }
};

folly::AtomicHashMap<uint64_t, folly::PackedSyncPtr<ResourceContract>> ahm(1024);

Task<std::pair<ResourceContract*, bool>, int> findOrCreateContract(uint64_t id) {
    folly::PackedSyncPtr<ResourceContract> pNewContract {};
    pNewContract.init();
    pNewContract.lock();

    auto[iter, bInserted] = ahm.insert({ id, pNewContract });
    auto& pContract = iter->second;
    if (bInserted) {
        auto contract = folly::makePromiseContract<void*>(Scheduler::get()->getCPUExecutor()->weakRef());
        pContract.set(new ResourceContract());
        pContract.unlock();
        co_return std::make_pair(iter->second.get(), true);
    } else {
        // Lock to make sure the contract pointer has been initialized in the above branch.
        pContract.lock();
        ResourceContract* pResult = pContract.get();
        pContract.unlock();
        co_return std::make_pair(pResult, false);
    }
}

// This can accept e.g. a callback that returns `MaterialInstance*`
Task<int, int> contractTest() {
    auto result = co_await findOrCreateContract(1);
    if (result) {
        auto[pContract, bInserted] = *result;
        if (bInserted) {
            print("1: new contract");

            // 0. await metadata
            // 1. load leaf dependencies @TODO circular dependencies?
            // 2. call callback that returns the resource ptr e.g. `ResourceInstance*`.
            // 3. resolve dependencies for `ResourceInstance`
            // 4. initialize `ResourceInstance` (e.g. load own data)
            // 5. expose subobjects somehow @TODO should subobjects be `ResourceInstance` or something else?

            auto* pRes = new MaterialResource();

            pContract->getPromise()->setValue(pRes);

            // load metadata
            // [optional] init resource object (e.g. add dependencies)
            // await dependencies
            // load own data
            // [optional] expose subobjects
            // resolve promise
            // return data
        } else {
            print("2: existing contract");
            // await future
            // return data
        }
    } else {
        print("3: failed to obtain contract");
    }

    co_return 0;
}

void deleteResource(uint64_t id) {
    folly::PackedSyncPtr<ResourceContract>& pContract = ahm.find(id)->second;
    pContract.lock();

    ResourceContract* contract = pContract.get();

    if (contract != nullptr) {
        delete contract;
        pContract.set(nullptr);
    }

    pContract.unlock();
}

int main(int argc, char* argv[]) {
    std::unique_ptr<Scheduler> schedMan = std::make_unique<Scheduler>(24, 24);
    std::unique_ptr<lemon::res::ResourceManager> resMan = std::make_unique<lemon::res::ResourceManager>(
        R"(C:\git\lemon\resources)");

    std::vector<TaskFuture<int, int>> tasks;
    for (int i = 0; i < 32; i++) {
        tasks.emplace_back(CPUTask(contractTest()));
    }

    auto result = folly::coro::blockingWait(folly::collectAll(tasks.begin(), tasks.end()));

    deleteResource(1);

    std::string path = "basketball.mat";
    auto meta = loadMetadata<MaterialResource>(path);

    if (meta) {
        auto* matData = (*meta).get<MaterialResource::Metadata>();
        print("load metadata: success");
    } else {
        print("load metadata: error");
    }

    return 0;
}
