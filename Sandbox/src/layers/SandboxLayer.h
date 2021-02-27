#pragma once
#include <Yggdrasil/core/util/layers/DearImguiLayer.h>

class SandboxLayer : public yggdrasil::ImguiLayer
{
public:
    SandboxLayer(const std::string& debugName);
    virtual ~SandboxLayer() {};

    virtual void onImguiUpdate() override;
    virtual void onUpdate() override;
    virtual void onEvent(yggdrasil::Event& event) override;
};
