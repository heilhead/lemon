#include <lemon/shared/HashStringStore.h>

using namespace lemon;

HashStringStore::HashStringStore(size_t sizeEst) : map{sizeEst} {}

HashStringStore::~HashStringStore()
{
    for (auto& [k, v] : map) {
        delete v.get();
    }
}

void
HashStringStore::insert(uint64_t hash, const std::string& str)
{
    folly::PackedSyncPtr<const std::string> ptr{};
    ptr.init();
    ptr.lock();

    auto [iter, bInserted] = map.insert({hash, std::move(ptr)});
    if (bInserted) {
        auto& ptr = iter->second;
        ptr.set(new std::string(str));
        ptr.unlock();
    }
}

const std::string*
HashStringStore::find(uint64_t hash)
{
    auto it = map.find(hash);
    if (it == map.end()) {
        return nullptr;
    } else {
        auto& ptr = it->second;
        ptr.lock();
        auto* str = ptr.get();
        ptr.unlock();
        return str;
    }
}
