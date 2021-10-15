#pragma once

#include <lemon/device/GPUDevice.h>
#include <lemon/device/Platform.h>
#include <lemon/device/Window.h>
#include <lemon/device/Input.h>
#include <lemon/shared/UnsafeSingleton.h>

namespace lemon::device {
    class Device : public UnsafeSingleton<Device> {
        Platform platform;
        Window window;
        Input input;
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

        inline Input*
        getInput()
        {
            return &input;
        }
    };
} // namespace lemon::device
