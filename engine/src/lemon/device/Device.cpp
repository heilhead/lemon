#include <lemon/device/Device.h>

using namespace lemon::device;

Device::Device() : platform{}, window{WindowDescriptor{}}, gpu{&platform, &window}, input{window.getHandle()}
{
}
