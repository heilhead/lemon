#include <lemon/tasks/filesystem.h>

using namespace lemon;
using namespace lemon::io;
using namespace lemon::scheduler;

Task<UnalignedMemory, Error>
lemon::io::coReadBinaryFile(const std::filesystem::path& filePath)
{
    co_return readBinaryFile(filePath);
}

Task<std::string, Error>
lemon::io::coReadTextFile(const std::filesystem::path& filePath)
{
    co_return readTextFile(filePath);
}
