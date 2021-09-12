#include <lemon/resource/types/MaterialResource.h>
#include <lemon/utils/utils.h>

using namespace lemon::res;
using namespace lemon::utils;

MaterialResource::MaterialResource() {
    print("MaterialResource constructor");
}

MaterialResource::~MaterialResource() {
    print("MaterialResource destructor");
}
