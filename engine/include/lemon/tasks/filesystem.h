#pragma once

#include <lemon/scheduler.h>

namespace lemon::io {
    scheduler::Task<UnalignedMemory, Error>
    coReadBinaryFile(const std::filesystem::path& filePath);

    scheduler::Task<std::string, Error>
    coReadTextFile(const std::filesystem::path& filePath);
} // namespace lemon::io
