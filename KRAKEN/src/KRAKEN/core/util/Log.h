#pragma once
#include <memory>
#include <spdlog/spdlog.h>

namespace kraken
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

#define KRAKEN_CORE_TRACE(...)    ::kraken::Logger::getCoreLogger()->trace(__VA_ARGS__);
#define KRAKEN_CORE_INFO(...)     ::kraken::Logger::getCoreLogger()->info(__VA_ARGS__);
#define KRAKEN_CORE_WARN(...)     ::kraken::Logger::getCoreLogger()->warn(__VA_ARGS__);
#define KRAKEN_CORE_ERROR(...)    ::kraken::Logger::getCoreLogger()->error(__VA_ARGS__);
#define KRAKEN_CORE_CRITICAL(...) ::kraken::Logger::getCoreLogger()->critical(__VA_ARGS__);

#define KRAKEN_TRACE(...)         ::kraken::Logger::getClientLogger()->trace(__VA_ARGS__);
#define KRAKEN_INFO(...)          ::kraken::Logger::getClientLogger()->info(__VA_ARGS__);
#define KRAKEN_WARN(...)          ::kraken::Logger::getClientLogger()->warn(__VA_ARGS__);
#define KRAKEN_ERROR(...)         ::kraken::Logger::getClientLogger()->error(__VA_ARGS__);
#define KRAKEN_CRITICAL(...)      ::kraken::Logger::getClientLogger()->critical(__VA_ARGS__);
