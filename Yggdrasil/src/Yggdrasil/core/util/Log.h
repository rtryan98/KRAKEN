#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace ygg
{
    class Logger
    {
    public:
        static void init();
        static void free();

        inline static std::shared_ptr<spdlog::logger>& getCoreLogger() { return coreLogger; }
        inline static std::shared_ptr<spdlog::logger>& getClientLogger() { return clientLogger; }
        inline static std::shared_ptr<spdlog::logger>& getValidationErrorLogger() { return validationErrorLogger; }

    private:
        Logger() = default;
        ~Logger() = default;
    private:
        // TODO: find a better way to do that
        static std::shared_ptr<spdlog::logger> coreLogger;
        static std::shared_ptr<spdlog::logger> clientLogger;
        static std::shared_ptr<spdlog::logger> validationErrorLogger;
    };
}

#define YGGDRASIL_CORE_TRACE(...)    ::ygg::Logger::getCoreLogger()->trace(__VA_ARGS__);
#define YGGDRASIL_CORE_INFO(...)     ::ygg::Logger::getCoreLogger()->info(__VA_ARGS__);
#define YGGDRASIL_CORE_WARN(...)     ::ygg::Logger::getCoreLogger()->warn(__VA_ARGS__);
#define YGGDRASIL_CORE_ERROR(...)    ::ygg::Logger::getCoreLogger()->error(__VA_ARGS__);
#define YGGDRASIL_CORE_CRITICAL(...) ::ygg::Logger::getCoreLogger()->critical(__VA_ARGS__);

#define YGGDRASIL_TRACE(...)         ::ygg::Logger::getClientLogger()->trace(__VA_ARGS__);
#define YGGDRASIL_INFO(...)          ::ygg::Logger::getClientLogger()->info(__VA_ARGS__);
#define YGGDRASIL_WARN(...)          ::ygg::Logger::getClientLogger()->warn(__VA_ARGS__);
#define YGGDRASIL_ERROR(...)         ::ygg::Logger::getClientLogger()->error(__VA_ARGS__);
#define YGGDRASIL_CRITICAL(...)      ::ygg::Logger::getClientLogger()->critical(__VA_ARGS__);
