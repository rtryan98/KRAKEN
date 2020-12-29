#include "KRAKEN/core/util/Layer.h"

namespace KRKN
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
