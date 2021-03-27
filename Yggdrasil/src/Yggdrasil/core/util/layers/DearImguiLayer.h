#pragma once
#include "Yggdrasil/core/util/Layer.h"

namespace ygg
{
    class ImguiLayer : public Layer
    {
    public:
        ImguiLayer(const std::string& debugName);
        virtual ~ImguiLayer() {};

        virtual void onAttach() override;

        virtual void onDetach() override;

        void beginFrame();
        void endFrame();
    private:
        static void init();
        static void free();
    private:
        friend class Application;
    };
}
