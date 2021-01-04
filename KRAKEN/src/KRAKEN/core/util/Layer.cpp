#include "KRAKEN/core/util/Layer.h"

namespace kraken
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
