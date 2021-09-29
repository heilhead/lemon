#include <lemon/device/Device.h>
#include <cassert>

using namespace lemon::device;

static Device* gInstance;

Device::Device() : platform{}, window{WindowDescriptor{}}, gpu{&platform, &window}
{
    assert(gInstance == nullptr);
    gInstance = this;
}

Device::~Device()
{
    gInstance = nullptr;
}

Device*
Device::get()
{
    return gInstance;
}