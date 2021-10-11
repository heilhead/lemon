#pragma once

#include <filesystem>
#include <string>
#include <tl/expected.hpp>
#include <lemon/shared/Memory.h>

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

    tl::expected<lemon::UnalignedMemory, Error>
    readBinaryFile(const std::filesystem::path& filePath);

    tl::expected<std::string, Error>
    readTextFile(const std::filesystem::path& filePath);
} // namespace lemon::io
