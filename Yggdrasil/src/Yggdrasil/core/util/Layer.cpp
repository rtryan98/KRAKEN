#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/util/Layer.h"

namespace ygg
{
    Layer::Layer(const std::string& debugName)
        : debugName( debugName )
    {

    }

    const std::string& Layer::getDebugName() const
    {
        return debugName;
    }

    bool Layer::isDetach() const
    {
        return this->detached;
    }

    void Layer::onDetachInternal()
    {
        this->detached = true;
        onDetach();
    }
}
