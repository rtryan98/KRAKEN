#pragma once
#include <string>

namespace KRKN
{
    class Layer
    {
    public:
        Layer(const std::string& debugName);
        ~Layer() = default;

        virtual void onAttach() = 0;
        virtual void onDetach() = 0;
        virtual void onUpdate() = 0;

        const std::string& getDebugName() const;
    private:
        std::string debugName{};
    };
}
