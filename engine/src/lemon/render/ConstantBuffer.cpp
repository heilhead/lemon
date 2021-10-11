#include <lemon/render/ConstantBuffer.h>
#include <lemon/render/utils.h>
#include <lemon/shared/math.h>

using namespace lemon;
using namespace lemon::render;

ConstantBuffer::ConstantBuffer(size_t inSize) : data{}, buffer{}
{
    size = inSize;
    offset = 0;
    data.allocate(size);
}

void
ConstantBuffer::init(wgpu::Device& device)
{
    wgpu::BufferDescriptor descriptor;
    descriptor.size = size;
    descriptor.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
    buffer = std::move(device.CreateBuffer(&descriptor));
}

uint32_t
ConstantBuffer::write(const uint8_t* inData, size_t inSize)
{
    LEMON_ASSERT(offset + inSize <= size);

    auto dataOffset = offset;
    auto err = memcpy_s(data.get(offset), inSize, inData, inSize);

    LEMON_ASSERT(!err, "failed to write constant buffer data");

    offset += math::alignUp(inSize, (size_t)kMinUniformBufferOffsetAlignment);

    return dataOffset;
}

void
ConstantBuffer::reset()
{
    offset = 0;
}

void
ConstantBuffer::upload(wgpu::Device& device) const
{
    device.GetQueue().WriteBuffer(buffer, 0, data, offset);
}
