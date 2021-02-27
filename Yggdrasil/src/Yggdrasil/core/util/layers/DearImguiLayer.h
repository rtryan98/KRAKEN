#pragma once
#include "Yggdrasil/core/util/Layer.h"
#include <vulkan/vulkan.h>

namespace yggdrasil
{
    /// <summary>
    /// Utility class for creating an UI Layer using Dear Imgui.
    /// Only one DearImgui Layer may be active at all times.
    /// For extended usage, Widgets shall be used or created.
    /// </summary>
    class ImguiLayer : public Layer
    {
    public:
        ImguiLayer(const std::string& debugName);
        virtual ~ImguiLayer() {};

        virtual void onAttach() final override;
        virtual void postAttach() {};

        virtual void onDetach() final override;
        virtual void preDetach() {};

        void beginFrame();
        void endFrame();
    private:
        VkDescriptorPool imguiPool{};
        VkRenderPass imguiRenderPass{};
    };
}
