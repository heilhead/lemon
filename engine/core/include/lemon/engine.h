#pragma once

//#include <dawn/webgpu_cpp.h>

//wgpu::ImageCopyBuffer CreateImageCopyBuffer(wgpu::Buffer buffer,
//    uint64_t offset,
//    uint32_t bytesPerRow,
//    uint32_t rowsPerImage = wgpu::kCopyStrideUndefined);
//wgpu::ImageCopyTexture CreateImageCopyTexture(
//    wgpu::Texture texture,
//    uint32_t level,
//    wgpu::Origin3D origin,
//    wgpu::TextureAspect aspect = wgpu::TextureAspect::All);
//wgpu::TextureDataLayout CreateTextureDataLayout(
//    uint64_t offset,
//    uint32_t bytesPerRow,
//    uint32_t rowsPerImage = wgpu::kCopyStrideUndefined);

class LemonEngine
{
public:
    void listDevices();
};
