#include "layers/SandboxLayer.h"
#include <Yggdrasil/Defines.h>
#include <imgui/imgui.h>

SandboxLayer::SandboxLayer(const std::string& debugName)
    : ImguiLayer(debugName)
{}

void SandboxLayer::onUpdate()
{

}

void SandboxLayer::onImguiUpdate()
{
    bool_t show{ true };
    beginFrame();
    ImGui::ShowDemoWindow(&show);
    endFrame();
}

void SandboxLayer::onEvent(yggdrasil::Event& event)
{
    YGGDRASIL_UNUSED_VARIABLE(event);
}
