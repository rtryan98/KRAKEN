#pragma once
#include <Yggdrasil/core/util/layers/DearImguiLayer.h>

class SandboxLayer : public ygg::ImguiLayer
{
public:
    SandboxLayer(const std::string& debugName);
    virtual ~SandboxLayer() {};

    virtual void onImguiUpdate() override;
    virtual void onUpdate() override;
    virtual void onEvent(ygg::Event& event) override;
private:
    void showPerformance();
};
