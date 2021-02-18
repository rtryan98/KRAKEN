#pragma once
#include <string>
#include "Yggdrasil/core/event/Event.h"

namespace yggdrasil
{
    class Layer
    {
    public:
        Layer(const std::string& debugName);
        virtual ~Layer() = default;

        virtual void onAttach() = 0;
        virtual void onDetach() = 0;
        virtual void onUpdate() = 0;
        virtual void onEvent(Event& event) = 0;

        const std::string& getDebugName() const;
    private:
        std::string debugName{};
    };
}
