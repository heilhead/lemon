#include <lemon/render/DebugUI.h>
#include <lemon/device/Device.h>

using namespace lemon;
using namespace lemon::render;
using namespace lemon::device;

bool
DebugUI::isEnabled() const
{
    return bEnabled;
}

void
DebugUI::enable()
{
    if (bEnabled) {
        return;
    }

    bEnabled = true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // io.IniFilename = NULL;

    // ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    auto* pDevice = device::Device::get();

    ImGui_ImplGlfw_InitForOther(pDevice->getWindow()->getHandle(), true);
    ImGui_ImplWGPU_Init(pDevice->getGPU()->getDevice().Get(), 2, WGPUTextureFormat_RGBA8Unorm);

    // TODO: Support resizable window?
    ImGui_ImplWGPU_CreateDeviceObjects();
}

void
DebugUI::disable()
{
    if (bEnabled) {
        bEnabled = false;

        ImGui_ImplWGPU_Shutdown();
    }
}

void
DebugUI::update() const
{
    if (bEnabled) {
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
}

void
DebugUI::render(const wgpu::RenderPassEncoder& pass) const
{
    if (bEnabled) {
        ImGui::Render();
        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass.Get());
    }
}
