#include <iostream>
#include <vector>

#include <dawn/dawn_proc.h>
#include <dawn/webgpu_cpp.h>
#include <dawn_native/D3D12Backend.h>
#include "dawn/webgpu.h"

#include "lemon/engine.h"

void LemonEngine::ListDevices()
{
    dawn_native::Instance instance;
    instance.DiscoverDefaultAdapters();
    wgpu::AdapterProperties properties;
    std::vector<dawn_native::Adapter> adapters = instance.GetAdapters();
    for (auto it = adapters.begin(); it != adapters.end(); ++it) {
        it->GetProperties(&properties);
        std::cout << properties.name << std::endl;
    }
}
