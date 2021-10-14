#pragma once

#include <lemon/device/GPUDevice.h>
#include <lemon/device/Platform.h>
#include <lemon/device/Window.h>
#include <lemon/shared/UnsafeSingleton.h>

namespace lemon::device {
    class Device : public UnsafeSingleton<Device> {
        Platform platform;
        Window window;
        GPUDevice gpu;

    public:
        Device();

        inline GPUDevice*
        getGPU()
        {
            return &gpu;
        }

        inline Platform*
        getPlatform()
        {
            return &platform;
        }

        inline Window*
        getWindow()
        {
            return &window;
        }
    };
} // namespace lemon::device
