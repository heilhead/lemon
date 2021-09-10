#include "lemon/shared/filesystem.h"

tl::expected<lemon::HeapBuffer, lemon::io::Error> lemon::io::readFile(const std::filesystem::path& filePath) {
    std::error_code sizeError;
    auto fileSize = std::filesystem::file_size(filePath, sizeError);
    if (sizeError) {
        return tl::make_unexpected(Error::InvalidFileSize);
    }

    auto filePathStr = filePath.string();
    std::FILE* handle;
    auto openError = fopen_s(&handle, filePathStr.c_str(), "rb");
    if (openError) {
        return tl::make_unexpected(Error::ReadFailed);
    }

    lemon::HeapBuffer buffer(fileSize);
    auto readBytes = std::fread(*buffer, 1, fileSize, handle);
    fclose(handle);

    if (readBytes != fileSize) {
        return tl::make_unexpected(Error::InvalidReadSize);
    }

    return buffer;
}