#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace yggdrasil
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

#define KRAKEN_CORE_TRACE(...)    ::yggdrasil::Logger::getCoreLogger()->trace(__VA_ARGS__);
#define KRAKEN_CORE_INFO(...)     ::yggdrasil::Logger::getCoreLogger()->info(__VA_ARGS__);
#define KRAKEN_CORE_WARN(...)     ::yggdrasil::Logger::getCoreLogger()->warn(__VA_ARGS__);
#define KRAKEN_CORE_ERROR(...)    ::yggdrasil::Logger::getCoreLogger()->error(__VA_ARGS__);
#define YGGDRASIL_CORE_CRITICAL(...) ::yggdrasil::Logger::getCoreLogger()->critical(__VA_ARGS__);

#define KRAKEN_TRACE(...)         ::yggdrasil::Logger::getClientLogger()->trace(__VA_ARGS__);
#define KRAKEN_INFO(...)          ::yggdrasil::Logger::getClientLogger()->info(__VA_ARGS__);
#define KRAKEN_WARN(...)          ::yggdrasil::Logger::getClientLogger()->warn(__VA_ARGS__);
#define KRAKEN_ERROR(...)         ::yggdrasil::Logger::getClientLogger()->error(__VA_ARGS__);
#define KRAKEN_CRITICAL(...)      ::yggdrasil::Logger::getClientLogger()->critical(__VA_ARGS__);
