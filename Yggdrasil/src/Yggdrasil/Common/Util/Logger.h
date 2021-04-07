#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace Ygg
{
    class Logger
    {
    public:
        static void Init();
        static void Shutdown();

        static std::shared_ptr<spdlog::logger> engineLogger;
        static std::shared_ptr<spdlog::logger> validationLogger;
    };
}

#define YGG_TRACE(...)    Ygg::Logger::engineLogger->trace(__VA_ARGS__);
#define YGG_INFO(...)     Ygg::Logger::engineLogger->info(__VA_ARGS__);
#define YGG_WARN(...)     Ygg::Logger::engineLogger->warn(__VA_ARGS__);
#define YGG_ERROR(...)    Ygg::Logger::engineLogger->error(__VA_ARGS__);
#define YGG_CRITICAL(...) Ygg::Logger::engineLogger->critical(__VA_ARGS__);
