#include "KRAKEN/core/util/Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace kraken
{
    std::shared_ptr<spdlog::logger> Logger::coreLogger;
    std::shared_ptr<spdlog::logger> Logger::clientLogger;
    std::shared_ptr<spdlog::logger> Logger::validationErrorLogger;

    void Logger::init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");
        coreLogger = spdlog::stdout_color_mt("KRAKEN Engine");
        coreLogger->set_level(spdlog::level::trace);
        clientLogger = spdlog::stdout_color_mt("APPLICATION");
        clientLogger->set_level(spdlog::level::trace);
        validationErrorLogger = spdlog::stdout_color_mt("Vulkan");
        validationErrorLogger->set_level(spdlog::level::trace);
    }

    void Logger::free()
    {
        spdlog::drop_all();
        coreLogger.reset();
        coreLogger = nullptr;
        clientLogger.reset();
        clientLogger = nullptr;
        validationErrorLogger.reset();
        validationErrorLogger = nullptr;
    }
}
