#pragma once

#include <filesystem>
#include <folly/hash/Hash.h>
#include <lemon/resource/common.h>
#include <lemon/shared/HashStringStore.h>
#include <lemon/shared/logger.h>

namespace lemon::res {
    static const char* kLocationObjectDelimiter = ":";

    struct ResourceLocation {
        ResourceHandle handle;
        ResourceObjectHandle object;

        explicit ResourceLocation(const std::string& inLocation);
        ResourceLocation(const std::string& inLocation, std::string& inObject);

        inline const std::string&
        getFileName() const
        {
            LEMON_ASSERT(handle.isValid());
            auto* ptr = gLookupMap.find(handle);
            LEMON_ASSERT(ptr != nullptr);
            return *ptr;
        }

        inline const std::string&
        getObjectName() const
        {
            LEMON_ASSERT(object.isValid());
            auto* ptr = gLookupMap.find(object);
            LEMON_ASSERT(ptr != nullptr);
            return *ptr;
        }

    private:
        inline static HashStringStore gLookupMap{512};

    private:
        inline static uint64_t
        createHash(const std::string& str)
        {
            return gLookupMap.insert(str);
        }
    };
} // namespace lemon::res
