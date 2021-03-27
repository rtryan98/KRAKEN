#pragma once
#include <string>
#include "Yggdrasil/core/event/Event.h"
#include "Yggdrasil/Types.h"

namespace ygg
{
    class Layer
    {
    public:
        Layer(const std::string& debugName);
        virtual ~Layer() = default;

        virtual void onAttach() = 0;
        virtual void onDetach() = 0;
        virtual void onUpdate() = 0;
        virtual void onImguiUpdate() {};

        virtual void onDetachInternal() final;

        virtual void onEvent(Event& event) = 0;

        const std::string& getDebugName() const;

        bool_t isDetach() const;
    private:
        std::string debugName{};
        bool_t detached{ false };
    };
}
