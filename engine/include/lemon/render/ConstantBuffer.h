#pragma once

#include <lemon/shared.h>

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

#include <folly/small_vector.h>

namespace lemon::render {
    class ConstantBuffer {
        lemon::HeapBuffer data;
        wgpu::Buffer buffer;
        wgpu::BindGroup bg;
        size_t offset;
        size_t size;

    public:
        ConstantBuffer(size_t inSize);

        void
        init(wgpu::Device& device);

        template<typename TData>
        uint32_t
        // std::enable_if_t<std::is_trivially_copyable_v<TData>, uint32_t>
        write(TData& inData)
        {
            auto szData = sizeof(inData);
            assert(offset + szData <= size);
            auto dataOffset = offset;
            memcpy(data.get<uint32_t>() + offset, &inData, szData);
            offset += alignup(szData, 256);
            return dataOffset;
        }

        uint32_t
        alignup(uint32_t val, uint32_t align)
        {
            return (val + align - 1) / align * align;
        }

        void
        reset();

        void
        upload(wgpu::Device& device) const;

        inline wgpu::BindGroup
        getBindGroup() const
        {
            return bg;
        }

        inline wgpu::Buffer
        getBuffer() const
        {
            return buffer;
        }
    };

    struct ConstantBufferBinding {
        ConstantBuffer* buffer = nullptr;
        uint32_t groupIndex = 0;
        uint32_t offset = 0;
    };

    struct ConstantBufferBindingLayout {
        folly::small_vector<ConstantBufferBinding, 12> bindings;

        template<typename TData>
        void
        addData(ConstantBuffer& buffer, uint32_t groupIndex, TData& data)
        {
            bindings.emplace_back(ConstantBufferBinding{
                .buffer = &buffer, .groupIndex = groupIndex, .offset = buffer.write(data)});
        }

        void
        reset()
        {
            bindings.clear();
        }

        void
        bind(wgpu::RenderPassEncoder& pass) const
        {
            for (auto& binding : bindings) {
                pass.SetBindGroup(binding.groupIndex, binding.buffer->getBindGroup(), 1, &binding.offset);
            }
        }
    };
} // namespace lemon::render