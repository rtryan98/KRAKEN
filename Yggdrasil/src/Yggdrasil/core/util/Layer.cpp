#include "Yggdrasil/pch.h"

#include "Yggdrasil/core/util/Layer.h"

namespace yggdrasil
{
    Layer::Layer(const std::string& debugName)
        : debugName( debugName )
    {

    }

    const std::string& Layer::getDebugName() const
    {
        return debugName;
    }
}
