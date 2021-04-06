#pragma once
#include "Yggdrasil/Common/Window/Window.h"

#include <vector>

namespace Ygg
{
    struct InitInfo
    {
        WindowCreateInfo windowCreateInfo{};
        struct FunctionInfo
        {
            std::vector<void(*)()> updateFunctions{};
            std::vector<void(*)()> initFunctions{};
            std::vector<void(*)()> shutdownFunctions{};
        } functionInfo;
    };

    void Init(const InitInfo* initInfo);
    void Run();
    void Shutdown();
}
