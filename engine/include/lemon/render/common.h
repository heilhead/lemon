#pragma once

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

    static constexpr uint8_t kMaxRenderFramesInFlight = 2;
    static constexpr uint8_t kSurfaceSharedBindGroupIndex = 0;
    static constexpr uint8_t kPostProcessSharedBindGroupIndex = 0;
    static constexpr uint8_t kMaterialBindGroupIndex = 1;
    static constexpr uint32_t kMinUniformBufferOffsetAlignment = 256;
    static constexpr uint32_t kConstantBufferSize = 4096 * 1024;

    static constexpr gsl::czstring<> kShaderSurfaceSharedGroupBlueprint =
        "internal/shaders/base/SurfaceBindGroup.wgsl";
    static constexpr gsl::czstring<> kShaderPostProcessSharedGroupBlueprint =
        "internal/shaders/base/PostProcessBindGroup.wgsl";

    static constexpr gsl::czstring<> kShaderDefinePipelineDepthOnly = "PIPELINE_DEPTH_ONLY";
    static constexpr gsl::czstring<> kShaderDefineMeshNormal = "MESH_ENABLE_NORMAL";
    static constexpr gsl::czstring<> kShaderDefineMeshTangent = "MESH_ENABLE_TANGENT";
    static constexpr gsl::czstring<> kShaderDefineMeshTangentSpace = "MESH_ENABLE_TANGENT_SPACE";
    static constexpr gsl::czstring<> kShaderDefineMeshTexture0 = "MESH_ENABLE_TEXTURE0";
    static constexpr gsl::czstring<> kShaderDefineMeshTexture1 = "MESH_ENABLE_TEXTURE1";
    static constexpr gsl::czstring<> kShaderDefineMeshSkinning = "MESH_ENABLE_SKINNING";
    static constexpr gsl::czstring<> kShaderDefineMaterialLighting = "MATERIAL_ENABLE_LIGHTING";

    inline size_t
    getVertexFormatSize(wgpu::VertexFormat format)
    {
        return gVertexFormatSize[format];
    }

    enum class RenderPassError { Unknown };
    enum class FrameRenderError { Unknown };

    struct RenderPassResources {
        wgpu::TextureView colorTargetView;
        wgpu::TextureView depthStencilView;
        wgpu::TextureView swapChainBackbufferView;
        // wgpu::BindGroup postProcessBindGroup;
    };

    struct RenderPassContext {
        RenderPassResources* pCurrentFrame{nullptr};
        RenderPassResources* pPreviousFrame{nullptr};
        uint8_t frameIndex{0};

        inline void
        swap(uint8_t inFrameIndex, RenderPassResources* pNextFrame,
             const wgpu::TextureView& nextFrameBackbufferView)
        {
            pPreviousFrame = pCurrentFrame;
            pCurrentFrame = pNextFrame;
            pCurrentFrame->swapChainBackbufferView = nextFrameBackbufferView;
            frameIndex = inFrameIndex;
        }
    };
} // namespace lemon::render
