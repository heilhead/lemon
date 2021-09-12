#include <lemon/resource/common.h>

using namespace lemon::res;

ResourceID lemon::res::getResourceID(std::string& file) {
    auto hash = folly::hash::fnva64(file);
//    if (object) {
//        hash = folly::hash::fnva64(*object, hash);
//    }
    return hash;
}
