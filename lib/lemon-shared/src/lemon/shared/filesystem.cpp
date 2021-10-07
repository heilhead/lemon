#include <lemon/shared/filesystem.h>
#include <fstream>

using namespace lemon;
using namespace lemon::io;

template<typename T>
using FileReadCallback = tl::expected<T, Error> (*)(std::ifstream& is, size_t size);

template<typename T>
tl::expected<T, Error>
readFile(const std::filesystem::path& filePath, FileReadCallback<T> callback) {
    if (std::ifstream is{filePath, std::ios::binary | std::ios::ate}) {
        auto size = is.tellg();
        if (size > 0) {
            is.seekg(0);
            return callback(is, size);
        } else {
            return tl::make_unexpected(Error::NoData);
        }
    } else {
        return tl::make_unexpected(Error::OpenFailed);
    }
}

tl::expected<HeapBuffer, Error>
lemon::io::readBinaryFile(const std::filesystem::path& filePath) {
    return readFile<HeapBuffer>(filePath, [](auto& is, auto size) -> tl::expected<HeapBuffer, Error> {
        HeapBuffer buffer(size);

        if (is.read(buffer.get<char>(), size)) {
            return buffer;
        } else {
            return tl::make_unexpected(Error::ReadFailed);
        }
    });
}

tl::expected<std::string, Error>
lemon::io::readTextFile(const std::filesystem::path& filePath) {
    return readFile<std::string>(filePath, [](auto& is, auto size) -> tl::expected<std::string, Error> {
        std::string buffer(size, '\0');

        if (is.read(&buffer[0], size)) {
            return buffer;
        } else {
            return tl::make_unexpected(Error::ReadFailed);
        }
    });
}