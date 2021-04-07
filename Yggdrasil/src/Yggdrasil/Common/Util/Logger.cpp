#include "Yggdrasil/pch.h"
#include "Yggdrasil/Common/Util/Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Ygg
{
    std::shared_ptr<spdlog::logger> Ygg::Logger::engineLogger = nullptr;
    std::shared_ptr<spdlog::logger> Ygg::Logger::validationLogger = nullptr;

    void Logger::Init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        Logger::engineLogger = spdlog::stdout_color_mt("Yggdrasil Engine");
        Logger::engineLogger->set_level(spdlog::level::trace);
        Logger::validationLogger = spdlog::stdout_color_mt("Vulkan");
        Logger::validationLogger->set_level(spdlog::level::trace);
    }

    void Logger::Shutdown()
    {
        spdlog::drop_all();
        Logger::engineLogger.reset();
        Logger::engineLogger = nullptr;
        Logger::validationLogger.reset();
        Logger::validationLogger = nullptr;
    }
}
