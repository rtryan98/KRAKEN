#pragma once
#include <string>
#include "KRAKEN/core/event/Event.h"

namespace kraken
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
