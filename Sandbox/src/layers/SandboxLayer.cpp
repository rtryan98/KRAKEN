#include "layers/SandboxLayer.h"
#include <Yggdrasil/Defines.h>
#include <Yggdrasil/core/Globals.h>
#include <Yggdrasil/core/Application.h>
#include <imgui/imgui.h>

SandboxLayer::SandboxLayer(const std::string& debugName)
    : ImguiLayer(debugName)
{}

void SandboxLayer::onUpdate()
{

}

void SandboxLayer::showPerformance()
{
    constexpr ImGuiWindowFlags flags
    {
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoMove
    };

    bool_t open{ true };
    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::Begin("Performance Stats", &open, flags);
    float_t ms{ ygg::globals::APPLICATION->getCpuFrametime() };
    ImGui::Text("Frametime: %.7f", ms);
    uint32_t fps{ ygg::globals::APPLICATION->getFramesPerSecond() };
    ImGui::Text("Frames per Second: %i", fps);
    ImGui::End();
}

void SandboxLayer::onImguiUpdate()
{
    beginFrame();
    showPerformance();
    endFrame();
}

void SandboxLayer::onEvent(ygg::Event& event)
{
    YGGDRASIL_UNUSED_VARIABLE(event);
}
