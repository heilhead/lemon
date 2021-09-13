#pragma once

#include <filesystem>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <variant>
#include <string>
#include <tl/expected.hpp>
#include <lemon/shared/DataBuffer.h>
#include <lemon/scheduler.h>

using namespace lemon::scheduler;

namespace lemon::io {
    enum class Error {
        // Failed to create IO stream.
        OpenFailed = 1,

        // Failed to read input stream.
        ReadFailed = 2,

        // Failed to write output stream.
        WriteFailed = 3,

        // Input stream is empty.
        NoData = 4,
    };

    tl::expected<lemon::HeapBuffer, Error>
    readBinaryFile(const std::filesystem::path& filePath);

    tl::expected<std::string, Error>
    readTextFile(const std::filesystem::path& filePath);

    Task<HeapBuffer, Error>
    coReadBinaryFile(const std::filesystem::path& filePath);

    Task<std::string, Error>
    coReadTextFile(const std::filesystem::path& filePath);
} // namespace lemon::io