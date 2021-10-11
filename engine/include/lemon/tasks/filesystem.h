#pragma once

#include <string>
#include <lemon/scheduler.h>
#include <lemon/shared/filesystem.h>

namespace lemon::io {
    scheduler::Task<UnalignedMemory, Error>
    coReadBinaryFile(const std::filesystem::path& filePath);

    scheduler::Task<std::string, Error>
    coReadTextFile(const std::filesystem::path& filePath);
} // namespace lemon::io
