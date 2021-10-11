#include "mesh.h"

#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/animation.h"

#include <lemon/shared/utils.h>
#include <lemon/engine.h>
#include <lemon/device/Device.h>
#include <lemon/shared/filesystem.h>
#include <lemon/resources.h>
#include <lemon/resource/types/ModelResource.h>
#include <lemon/render/ComboRenderPassDescriptor.h>
#include <lemon/render/utils.h>
#include <lemon/render/ConstantBuffer.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <dawn/webgpu_cpp.h>
#include <dawn/webgpu_cpp_print.h>
#include <dawn/dawn_proc.h>
#include <dawn_native/D3D12Backend.h>

using namespace lemon;
using namespace lemon::device;
using namespace lemon::utils;
using namespace lemon::res;
using namespace lemon::scheduler;
using namespace lemon::render;

const ModelResource::Model*
loadModel()
{
    ResourceLocation location(R"(ozz-sample\MannequinSkeleton.lem:SK_Mannequin)");

    auto result = Scheduler::get()->block(
        ResourceManager::get()->loadResource<ModelResource>(location, ResourceLifetime::Static));

    return (*result)->getObject<ModelResource::Model>(location.object);
}

class MiniRender {
public:
    MiniRender() : cbuffer{4096} {}

private:
    std::unique_ptr<lemon::device::Platform> platform;

    wgpu::Device device;
    wgpu::Buffer indexBuffer;
    wgpu::Buffer vertexBuffer;
    wgpu::Texture texture;
    wgpu::Sampler sampler;
    wgpu::Queue queue;
    wgpu::SwapChain swapChain;
    wgpu::TextureView depthStencilView;
    wgpu::RenderPipeline pipeline;
    wgpu::BindGroup bindGroup;
    wgpu::BindGroup bindGroupShared;
    wgpu::Buffer sharedUniformBuffer;

    ConstantBuffer cbuffer;

    const model::ModelMesh* mesh;

public:
    void
    initTextures()
    {
        wgpu::TextureDescriptor descriptor;
        descriptor.dimension = wgpu::TextureDimension::e2D;
        descriptor.size.width = 1024;
        descriptor.size.height = 1024;
        descriptor.size.depthOrArrayLayers = 1;
        descriptor.sampleCount = 1;
        descriptor.format = wgpu::TextureFormat::RGBA8Unorm;
        descriptor.mipLevelCount = 1;
        descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::TextureBinding;
        texture = device.CreateTexture(&descriptor);

        sampler = device.CreateSampler();

        // Initialize the texture with arbitrary data until we can load images
        std::vector<uint8_t> data(4 * 1024 * 1024, 0);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = static_cast<uint8_t>(i % 253);
        }

        wgpu::Buffer stagingBuffer = createBufferFromData(
            device, data.data(), static_cast<uint32_t>(data.size()), wgpu::BufferUsage::CopySrc);
        wgpu::ImageCopyBuffer imageCopyBuffer = createImageCopyBuffer(stagingBuffer, 0, 4 * 1024);
        wgpu::ImageCopyTexture imageCopyTexture = createImageCopyTexture(texture, 0, {0, 0, 0});
        wgpu::Extent3D copySize = {1024, 1024, 1};

        wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
        encoder.CopyBufferToTexture(&imageCopyBuffer, &imageCopyTexture, &copySize);

        wgpu::CommandBuffer copy = encoder.Finish();
        queue.Submit(1, &copy);
    }

    void
    createPipeline(const DawnSwapChainImplementation& swapChainImpl, uint32_t windowWidth,
                   uint32_t windowHeight)
    {
        glm::mat4 matIdentity;

        glm::mat4 matModel = glm::translate(matIdentity, glm::vec3(0.f, -1.f, 0.f)) *
                             glm::scale(matIdentity, glm::vec3(0.01f, 0.01f, 0.01f)) *
                             glm::rotate(matIdentity, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));

        glm::mat4 matView = glm::translate(matIdentity, glm::vec3(0.f, 0.f, -1.f)) *
                            glm::rotate(matIdentity, glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));

        glm::mat4 matProjection =
            glm::perspective(90.f, (float)windowWidth / (float)windowHeight, 0.f, 150.f);
        glm::mat4 matViewProjection = matProjection * matView * matModel;

        sharedUniformBuffer = createBufferFromData(device, (void*)&matViewProjection, sizeof(glm::mat4),
                                                   wgpu::BufferUsage::Uniform);
        auto bgl0 =
            makeBindGroupLayout(device, {{0, wgpu::ShaderStage::Vertex, wgpu::BufferBindingType::Uniform}});
        bindGroupShared = makeBindGroup(device, bgl0, {{0, sharedUniformBuffer}});

        wgpu::TextureView view = texture.CreateView();

        auto bgl1 = makeBindGroupLayout(
            device, {
                        {0, wgpu::ShaderStage::Fragment, wgpu::SamplerBindingType::Filtering},
                        {1, wgpu::ShaderStage::Fragment, wgpu::TextureSampleType::Float},
                    });
        bindGroup = makeBindGroup(device, bgl1, {{0, sampler}, {1, view}});

        auto bgl2 =
            makeBindGroupLayout(device, {
                                            {0, wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment,
                                             wgpu::BufferBindingType::Uniform, true},
                                        });

        wgpu::BindGroupLayout bgLayouts[2] = {bgl0, bgl1};

        auto shaderSource =
            *lemon::io::readTextFile(R"(C:\git\lemon\resources\ozz-sample\shader-basic.wgsl)");
        auto shaderModule = createShaderModule(device, shaderSource.c_str());

        wgpu::PipelineLayoutDescriptor plLayoutDesc;
        plLayoutDesc.bindGroupLayoutCount = 2;
        plLayoutDesc.bindGroupLayouts = bgLayouts;

        wgpu::RenderPipelineDescriptor desc;
        desc.layout = device.CreatePipelineLayout(&plLayoutDesc);

        // vertex state
        //{
        wgpu::VertexBufferLayout vertexLayout;
        vertexLayout.stepMode = wgpu::VertexStepMode::Vertex;
        vertexLayout.arrayStride = mesh->vertexFormat.getStride();
        vertexLayout.attributeCount = mesh->vertexFormat.getAttributeCount();
        vertexLayout.attributes = mesh->vertexFormat.getAttributes();

        auto& vertexState = desc.vertex;
        vertexState.bufferCount = 1;
        vertexState.buffers = &vertexLayout;
        vertexState.entryPoint = "vs_main";
        vertexState.module = shaderModule;
        //}

        // fragment state
        //{
        wgpu::BlendComponent blendComponent;
        blendComponent.srcFactor = wgpu::BlendFactor::One;
        blendComponent.dstFactor = wgpu::BlendFactor::Zero;
        blendComponent.operation = wgpu::BlendOperation::Add;

        wgpu::BlendState blendState;
        blendState.color = blendComponent;
        blendState.alpha = blendComponent;

        wgpu::ColorTargetState target;
        target.format = static_cast<wgpu::TextureFormat>(
            dawn_native::d3d12::GetNativeSwapChainPreferredFormat(&swapChainImpl));
        target.blend = &blendState;
        target.writeMask = wgpu::ColorWriteMask::All;

        wgpu::FragmentState fragmentState;
        fragmentState.targetCount = 1;
        fragmentState.targets = &target;
        fragmentState.entryPoint = "fs_main";
        fragmentState.module = shaderModule;

        desc.fragment = &fragmentState;
        //}

        // primitive state
        //{
        auto& primitiveState = desc.primitive;
        primitiveState.topology = wgpu::PrimitiveTopology::TriangleList;
        primitiveState.stripIndexFormat = wgpu::IndexFormat::Undefined;
        primitiveState.frontFace = wgpu::FrontFace::CW;
        primitiveState.cullMode = wgpu::CullMode::None;
        //}

        // depth-stencil state
        //{
        wgpu::StencilFaceState faceState;
        faceState.compare = wgpu::CompareFunction::Always;
        faceState.failOp = wgpu::StencilOperation::Keep;
        faceState.depthFailOp = wgpu::StencilOperation::Keep;
        faceState.passOp = wgpu::StencilOperation::Keep;

        wgpu::DepthStencilState stencilState;
        stencilState.stencilFront = faceState;
        stencilState.stencilBack = faceState;
        stencilState.format = wgpu::TextureFormat::Depth24PlusStencil8;
        stencilState.depthWriteEnabled = false;
        stencilState.depthCompare = wgpu::CompareFunction::Always;
        stencilState.stencilReadMask = 0xFF;
        stencilState.stencilWriteMask = 0xFF;
        stencilState.depthBias = 0;
        stencilState.depthBiasSlopeScale = 0.0;
        stencilState.depthBiasClamp = 0.0;

        desc.depthStencil = &stencilState;
        //}

        // multi-sample state
        //{
        auto& multisampleState = desc.multisample;
        multisampleState.count = 1;
        multisampleState.mask = 0xFFFFFFFF;
        multisampleState.alphaToCoverageEnabled = false;
        //}

        pipeline = device.CreateRenderPipeline(&desc);
    }

    void
    init(Window* window)
    {
        glm::vec4 v(0.69291f, 0.66929f, -0.24409, 0.0f);
        glm::vec4 normalized = glm::normalize(v);

        auto* gpu = Device::get()->getGPU();

        device = gpu->getDevice();
        swapChain = gpu->getSwapChain();
        queue = gpu->getQueue();

        const auto& swapChainImpl = gpu->getSwapChainImpl();
        auto [wndWidth, wndHeight] = window->getSize();

        // stage 3

        initTextures();

        // stage 4

        depthStencilView = createDefaultDepthStencilView(device, wndWidth, wndHeight);

        auto model = loadModel();

        mesh = model->getMeshes()[0];

        vertexBuffer = createBufferFromData(device, mesh->vertexData, mesh->vertexData.size(),
                                            wgpu::BufferUsage::Vertex);
        indexBuffer = createBufferFromData(device, mesh->indexData, mesh->indexData.size(),
                                           wgpu::BufferUsage::Index);

        cbuffer.init(device);
        createPipeline(swapChainImpl, wndWidth, wndHeight);
    }

    void
    render()
    {
        // cbuffer.reset();
        // cbufLayout.reset();

        //{
        //    glm::f32vec4 v1(1.0f, 1.0f, 1.0f, 1.0f);
        //    cbufLayout.addData(cbuffer, 2, v1);

        //    glm::f32vec4 v2(0.5f, 0.5f, 0.5f, 1.0f);
        //    cbufLayout.addData(cbuffer, 3, v2);

        //    glm::f32vec4 v3(1.0f, 0.0f, 0.0f, 1.0f);
        //    cbufLayout.addData(cbuffer, 4, v3);
        //}

        wgpu::TextureView backbufferView = swapChain.GetCurrentTextureView();
        ComboRenderPassDescriptor renderPass({backbufferView}, depthStencilView);

        wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
        {
            wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);
            pass.SetPipeline(pipeline);
            pass.SetBindGroup(0, bindGroupShared);
            pass.SetBindGroup(1, bindGroup);

            // cbufLayout.bind(pass);

            pass.SetVertexBuffer(0, vertexBuffer);
            pass.SetIndexBuffer(indexBuffer, mesh->indexFormat);
            pass.DrawIndexed(mesh->indexCount);
            pass.EndPass();
        }

        wgpu::CommandBuffer commands = encoder.Finish();
        queue.Submit(1, &commands);
        swapChain.Present();
    }
};

/*

? additional scene information
    -> lighting
    -> etc.

? additional render packet information
    -> modelview matrix
    -> joint transform data

domain:
    -> surface
        -> lit
            -> include scene lighting data
        -> unlit

usage (internal, at material creation):
    -> static mesh
        -> include model-view matrix
    -> skeletal mesh
        -> include model-view matrix
        -> include joint transform data

[[binding(0), location(0)]]
SceneParamsUniform
    -> timer
    -> projection matrix

[[binding(0), location(1)]]
MeshParamsUniform
    -> model-view matrix

[[binding(0), location(2)]]
SkeletalMeshParamsUniform
    -> joint transform data

[[binding(0), location(3)]]
LightingParamsUniform
    -> light data array

[[binding(1), location(0)]] (reflection-based bindings)
sampler0

[[binding(1), location(1)]] (reflection-based bindings)
sampler1

[[binding(1), location(2)]] (reflection-based bindings)
texture0

[[binding(1), location(3)]] (reflection-based bindings)
texture1

material:

    -> baseType: Shader | Material
    -> basePath: [string path to either material or shader]

    [shader/template definitions]
    -> definitions [key (string) => value (string)]
            -> `SCROLL_SPEED_U` => `0.5`
            -> `SCROLL_SPEED_V` => `1.0`

    -> samplers
        -> generic [binding lookup]
            -> magFilter
            -> minFilter
            -> addressMode

    -> textures
        -> albedo [binding lookup]
            -> path/to/texture.dds
        -> normal [binding lookup]
            -> ...

*/

void
testMeshRendering()
{
    lemon::Engine engine;

    std::string assetPath(R"(C:\git\lemon\resources)");

    engine.init(assetPath);

    MiniRender render;

    render.init(Device::get()->getWindow());

    {
        engine.loop([&](float dt) {
            render.render();
            return LoopControl::Continue;
        });
    }

    engine.shutdown();
}
