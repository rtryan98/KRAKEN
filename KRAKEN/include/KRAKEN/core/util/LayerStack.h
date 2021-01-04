#pragma once
#include <vector>
#include "KRAKEN/core/util/Layer.h"

namespace kraken
{
    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack();


        void push(Layer* layer);
        void pop(Layer* layer);
        void pushOverlay(Layer* layer);
        void popOverlay(Layer* layer);
        std::vector<Layer*>::iterator begin();
        std::vector<Layer*>::iterator end();
        std::vector<Layer*>::reverse_iterator rbegin();
        std::vector<Layer*>::reverse_iterator rend();
        std::vector<Layer*>::const_iterator begin() const;
        std::vector<Layer*>::const_iterator end() const;
        std::vector<Layer*>::const_reverse_iterator rbegin() const;
        std::vector<Layer*>::const_reverse_iterator rend() const;
    private:
        uint32_t index{ 0 };
        std::vector<Layer*> layers;
    };
}
