#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace Ygg
{
    class CLogger
    {
    public:
        static void Init();
        static void Shutdown();

        static const std::shared_ptr<spdlog::logger>& GetEngineLogger();
        static const std::shared_ptr<spdlog::logger>& GetValidationLogger();

    private:
        static std::shared_ptr<spdlog::logger> s_pEngineLogger;
        static std::shared_ptr<spdlog::logger> s_pValidationLogger;
    };
}

#define YGG_TRACE(...)    Ygg::CLogger::GetEngineLogger()->trace(__VA_ARGS__);
#define YGG_INFO(...)     Ygg::CLogger::GetEngineLogger()->info(__VA_ARGS__);
#define YGG_WARN(...)     Ygg::CLogger::GetEngineLogger()->warn(__VA_ARGS__);
#define YGG_ERROR(...)    Ygg::CLogger::GetEngineLogger()->error(__VA_ARGS__);
#define YGG_CRITICAL(...) Ygg::CLogger::GetEngineLogger()->critical(__VA_ARGS__);
