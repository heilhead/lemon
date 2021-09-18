#pragma once

#include <filesystem>

namespace lemon::converter {
    void
    convert(std::filesystem::path inFile, std::filesystem::path outFile);
}