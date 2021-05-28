// Copyright 2021 Robert Ryan. See LICENCE.md.

#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Util/Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Ygg
{
    std::shared_ptr<spdlog::logger> Ygg::CLogger::s_pEngineLogger = nullptr;
    std::shared_ptr<spdlog::logger> Ygg::CLogger::s_pValidationLogger = nullptr;

    void CLogger::Init()
    {
        spdlog::set_pattern("[%T] %n: %^%v%$");
        CLogger::s_pEngineLogger = spdlog::stdout_color_mt("Yggdrasil");
        CLogger::s_pEngineLogger->set_level(spdlog::level::trace);
        CLogger::s_pValidationLogger = spdlog::stdout_color_mt("Vulkan");
        CLogger::s_pValidationLogger->set_level(spdlog::level::trace);
    }

    void CLogger::Shutdown()
    {
        spdlog::drop_all();
        CLogger::s_pEngineLogger.reset();
        CLogger::s_pEngineLogger = nullptr;
        CLogger::s_pValidationLogger.reset();
        CLogger::s_pValidationLogger = nullptr;
    }

    const std::shared_ptr<spdlog::logger>& CLogger::GetEngineLogger()
    {
        return CLogger::s_pEngineLogger;
    }

    const std::shared_ptr<spdlog::logger>& CLogger::GetValidationLogger()
    {
        return CLogger::s_pValidationLogger;
    }
}
