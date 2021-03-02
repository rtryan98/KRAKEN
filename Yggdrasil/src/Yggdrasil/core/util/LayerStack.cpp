#include "Yggdrasil/pch.h"
#include "Yggdrasil/core/util/Log.h"
#include "Yggdrasil/core/util/LayerStack.h"

namespace yggdrasil
{
    LayerStack::~LayerStack()
    {
        for (Layer* const layer : layers)
        {
            if (!layer->isDetach())
            {
                layer->onDetach();
            }
            delete layer;
        }
    }

    void LayerStack::push(Layer* layer)
    {
        layers.emplace(layers.begin() + index, layer);
        layer->onAttach();
        index++;
        YGGDRASIL_CORE_TRACE("Pushed layer '{0}'", layer->getDebugName());
    }

    void LayerStack::pop(Layer* layer)
    {
        auto it{ std::find(layers.begin(), layers.begin() + index, layer) };
        if (it != layers.begin() + index)
        {
            layer->onDetach();
            layers.erase(it);
            index--;
            YGGDRASIL_CORE_TRACE("Popped layer '{0}'", layer->getDebugName());
        }
    }

    void LayerStack::pushOverlay(Layer* layer)
    {
        layers.emplace_back(layer);
        layer->onAttach();
        YGGDRASIL_CORE_TRACE("Pushed layer [overlay] '{0}'", layer->getDebugName());
    }

    void LayerStack::popOverlay(Layer* layer)
    {
        auto it = std::find(layers.begin() + index, layers.end(), layer);
        if (it != layers.end())
        {
            layer->onDetach();
            layers.erase(it);
            YGGDRASIL_CORE_TRACE("Popped layer [overlay] '{0}'", layer->getDebugName());
        }
    }

    std::vector<Layer*>::iterator LayerStack::begin()
    {
        return layers.begin();
    }

    std::vector<Layer*>::iterator LayerStack::end()
    {
        return layers.end();
    }

    std::vector<Layer*>::reverse_iterator LayerStack::rbegin()
    {
        return layers.rbegin();
    }

    std::vector<Layer*>::reverse_iterator LayerStack::rend()
    {
        return layers.rend();
    }

    std::vector<Layer*>::const_iterator LayerStack::begin() const
    {
        return layers.begin();
    }

    std::vector<Layer*>::const_iterator LayerStack::end() const
    {
        return layers.end();
    }

    std::vector<Layer*>::const_reverse_iterator LayerStack::rbegin() const
    {
        return layers.rbegin();
    }

    std::vector<Layer*>::const_reverse_iterator LayerStack::rend() const
    {
        return layers.rend();
    }
}
