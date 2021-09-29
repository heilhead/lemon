#pragma once

#include <lemon/device/GPUDevice.h>
#include <lemon/device/Platform.h>
#include <lemon/device/Window.h>

namespace lemon::device {
    class Device {
        Platform platform;
        Window window;
        GPUDevice gpu;

    public:
        Device();
        ~Device();

        static Device*
        get();

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