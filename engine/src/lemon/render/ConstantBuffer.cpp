#include <lemon/render/ConstantBuffer.h>
#include <lemon/render/utils.h>

using namespace lemon::render;

ConstantBuffer::ConstantBuffer(size_t inSize)
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
    buffer = device.CreateBuffer(&descriptor);

    auto layout = makeBindGroupLayout(device, {{0, wgpu::ShaderStage::Fragment | wgpu::ShaderStage::Vertex,
                                                wgpu::BufferBindingType::Uniform, true}});
    bg = makeBindGroup(device, layout, {{0, buffer, 0, 256}});
}

void
ConstantBuffer::reset()
{
    offset = 0;
}

void
ConstantBuffer::upload(wgpu::Device& device) const
{
    device.GetQueue().WriteBuffer(buffer, 0, data.get<void>(), offset);
}