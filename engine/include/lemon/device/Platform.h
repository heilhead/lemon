#pragma once

#include <dawn_platform/DawnPlatform.h>

namespace lemon::device {
    class PlatformCache : public dawn_platform::CachingInterface {
        virtual size_t
        LoadData(const WGPUDevice device, const void* key, size_t keySize, void* valueOut,
                 size_t valueSize) override;

        virtual void
        StoreData(const WGPUDevice device, const void* key, size_t keySize, const void* value,
                  size_t valueSize) override;
    };

    class Platform : public dawn_platform::Platform {
        PlatformCache cache;

    public:
        Platform();

        virtual dawn_platform::CachingInterface*
        GetCachingInterface(const void* fingerprint, size_t fingerprintSize) override;
    };
} // namespace lemon::device
