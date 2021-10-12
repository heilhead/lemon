#pragma once

#include <unordered_map>
#include <webgpu/webgpu_cpp.h>

namespace lemon::render {
    namespace {
        static std::unordered_map<wgpu::VertexFormat, size_t> gVertexFormatSize = {
            {wgpu::VertexFormat::Undefined, 0},  {wgpu::VertexFormat::Uint8x2, 2},
            {wgpu::VertexFormat::Uint8x4, 4},    {wgpu::VertexFormat::Sint8x2, 2},
            {wgpu::VertexFormat::Sint8x4, 4},    {wgpu::VertexFormat::Unorm8x2, 2},
            {wgpu::VertexFormat::Unorm8x4, 4},   {wgpu::VertexFormat::Snorm8x2, 2},
            {wgpu::VertexFormat::Snorm8x4, 4},   {wgpu::VertexFormat::Uint16x2, 4},
            {wgpu::VertexFormat::Uint16x4, 8},   {wgpu::VertexFormat::Sint16x2, 4},
            {wgpu::VertexFormat::Sint16x4, 8},   {wgpu::VertexFormat::Unorm16x2, 4},
            {wgpu::VertexFormat::Unorm16x4, 8},  {wgpu::VertexFormat::Snorm16x2, 4},
            {wgpu::VertexFormat::Snorm16x4, 8},  {wgpu::VertexFormat::Float16x2, 4},
            {wgpu::VertexFormat::Float16x4, 8},  {wgpu::VertexFormat::Float32, 4},
            {wgpu::VertexFormat::Float32x2, 8},  {wgpu::VertexFormat::Float32x3, 12},
            {wgpu::VertexFormat::Float32x4, 16}, {wgpu::VertexFormat::Uint32, 4},
            {wgpu::VertexFormat::Uint32x2, 8},   {wgpu::VertexFormat::Uint32x3, 12},
            {wgpu::VertexFormat::Uint32x4, 16},  {wgpu::VertexFormat::Sint32, 4},
            {wgpu::VertexFormat::Sint32x2, 8},   {wgpu::VertexFormat::Sint32x3, 12},
            {wgpu::VertexFormat::Sint32x4, 16},
        };
    }
    
    static constexpr uint8_t kSharedBindGroupIndex = 0;
    static constexpr uint8_t kMaterialBindGroupIndex = 1;
    static constexpr uint32_t kMinUniformBufferOffsetAlignment = 256;
    static constexpr uint32_t kConstantBufferSize = 4096 * 1024;

    inline size_t
    getVertexFormatSize(wgpu::VertexFormat format)
    {
        return gVertexFormatSize[format];
    }
} // namespace lemon::render