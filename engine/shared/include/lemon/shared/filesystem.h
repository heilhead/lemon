#pragma once

#include <filesystem>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <variant>
#include <tl/expected.hpp>
#include "lemon/shared/DataBuffer.h"

namespace lemon::io {
    enum class Error {
        ReadFailed = 1,
        WriteFailed = 2,
        InvalidReadSize = 3,
        InvalidFileSize = 4,
    };

    tl::expected<HeapBuffer, Error> readFile(const std::filesystem::path& filePath);
}