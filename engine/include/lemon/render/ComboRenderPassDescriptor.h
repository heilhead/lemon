#pragma once

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

namespace lemon::render {
    static constexpr uint8_t kMaxColorAttachments = 8u;

    struct ComboRenderPassDescriptor : public wgpu::RenderPassDescriptor {
    public:
        ComboRenderPassDescriptor(std::initializer_list<wgpu::TextureView> colorAttachmentInfo,
                                  wgpu::TextureView depthStencil = wgpu::TextureView())
        {
            for (uint32_t i = 0; i < kMaxColorAttachments; ++i) {
                cColorAttachments[i].loadOp = wgpu::LoadOp::Clear;
                cColorAttachments[i].storeOp = wgpu::StoreOp::Store;
                cColorAttachments[i].clearColor = {0.2f, 0.2f, 0.2f, 0.0f};
            }

            cDepthStencilAttachmentInfo.clearDepth = 1.0f;
            cDepthStencilAttachmentInfo.clearStencil = 0;
            cDepthStencilAttachmentInfo.depthLoadOp = wgpu::LoadOp::Clear;
            cDepthStencilAttachmentInfo.depthStoreOp = wgpu::StoreOp::Store;
            cDepthStencilAttachmentInfo.stencilLoadOp = wgpu::LoadOp::Clear;
            cDepthStencilAttachmentInfo.stencilStoreOp = wgpu::StoreOp::Store;

            colorAttachmentCount = static_cast<uint32_t>(colorAttachmentInfo.size());
            uint32_t colorAttachmentIndex = 0;
            for (const wgpu::TextureView& colorAttachment : colorAttachmentInfo) {
                if (colorAttachment.Get() != nullptr) {
                    cColorAttachments[colorAttachmentIndex].view = colorAttachment;
                }
                ++colorAttachmentIndex;
            }
            colorAttachments = cColorAttachments.data();

            if (depthStencil.Get() != nullptr) {
                cDepthStencilAttachmentInfo.view = depthStencil;
                depthStencilAttachment = &cDepthStencilAttachmentInfo;
            } else {
                depthStencilAttachment = nullptr;
            }
        }

        ComboRenderPassDescriptor(const ComboRenderPassDescriptor& other)
        {
            *this = other;
        }

        const ComboRenderPassDescriptor&
        operator=(const ComboRenderPassDescriptor& otherRenderPass)
        {
            cDepthStencilAttachmentInfo = otherRenderPass.cDepthStencilAttachmentInfo;
            cColorAttachments = otherRenderPass.cColorAttachments;
            colorAttachmentCount = otherRenderPass.colorAttachmentCount;

            colorAttachments = cColorAttachments.data();

            if (otherRenderPass.depthStencilAttachment != nullptr) {
                // Assign desc.depthStencilAttachment to this->depthStencilAttachmentInfo;
                depthStencilAttachment = &cDepthStencilAttachmentInfo;
            } else {
                depthStencilAttachment = nullptr;
            }

            return *this;
        }

        std::array<wgpu::RenderPassColorAttachment, kMaxColorAttachments> cColorAttachments;
        wgpu::RenderPassDepthStencilAttachment cDepthStencilAttachmentInfo = {};
    };
} // namespace lemon::render
