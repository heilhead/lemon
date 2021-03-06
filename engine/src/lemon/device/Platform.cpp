#include <lemon/device/Platform.h>

using namespace lemon::device;

size_t
PlatformCache::LoadData(const WGPUDevice device, const void* key, size_t keySize, void* valueOut,
                        size_t valueSize)
{
    // const std::string strKey(reinterpret_cast<gsl::czstring<>>(key), keySize);
    return 0;
}

void
PlatformCache::StoreData(const WGPUDevice device, const void* key, size_t keySize, const void* value,
                         size_t valueSize)
{
    // TODO: Implement shader caching

    // const std::string strKey(reinterpret_cast<gsl::czstring<>>(key), keySize);
    // const uint8_t* pStart = reinterpret_cast<const uint8_t*>(value);
    // std::vector<uint8_t> data(pStart, pStart + valueSize);
}

Platform::Platform() {}

dawn_platform::CachingInterface*
Platform::GetCachingInterface(const void* fingerprint, size_t fingerprintSize)
{
    return &cache;
}
