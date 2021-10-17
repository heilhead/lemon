#pragma once

#include <lemon/render/common.h>

namespace lemon::render {
    class ConstantBuffer {
        UnalignedMemory data;
        wgpu::Buffer buffer;
        size_t offset;
        size_t size;

    public:
        ConstantBuffer(size_t inSize = kConstantBufferSize);

        void
        init(wgpu::Device& device);

        uint32_t
        write(const uint8_t* inData, size_t inSize);

        void
        reset();

        void
        upload(wgpu::Device& device) const;

        inline const wgpu::Buffer&
        getBuffer() const
        {
            return buffer;
        }
    };
} // namespace lemon::render
