#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
    #define YGGDRASIL_PLATFORM_WINDOWS 1
#elif __ANDROID__
    #define YGGDRASIL_PLATFORM_ANDROID 1
    #error "Android is not supported."
#elif __APPLE__
    #error "Apple is not supported."
    #define YGGDRASIL_PLATFORM_APPLE 1
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
        #error "Iphone is not supported!"
    #elif TARGET_OS_IPHONE
        #error "Iphone is not supported!"
    #elif TARGET_OS_MAC
        #define YGGDRASIL_PLATFORM_OSX 1
    #else
        #error "Unsupported Apple platform"
    #endif
#elif __linux__ || __linux || linux || __gnu_linux__
    #define YGGDRASIL_PLATFORM_LINUX 1
    #error "Linux is not supported."
#elif __unix__ || __unix
    #define YGGDRASIL_PLATFORM_UNKNOWN_UNIX 1
    #error "Unknown Unix platform."
#elif defined(_POSIX_VERSION) || __posix
    #define YGGDRASIL_PLATFORM_UNKNOWN_POSIX 1
    #error "Unsupported Posix platform."
#else
    #error "Unknown platform."
#endif

#define YGGDRASIL_UNUSED_VARIABLE(x) x

#if YGGDRASIL_PLATFORM_WINDOWS
    #define FORCE_INLINE __forceinline
    #define FORCE_NO_INLINE _declspec(noinline)
#else
    #define FORCE_INLINE inline
    #define FORCE_NO_INLINE
#endif

#if _MSC_VER
    #include <intrin.h>
    #define debugBreak() __debugbreak()
    #elif YGGDRASIL_PLATFORM_ANDROID || YGGDRASIL_PLATFORM_APPLE || YGGDRASIL_PLATFORM_LINUX
    #include <unistd.h>
    #if _POSIX_VERSION
        #include <signal.h>
        #define debugBreak() raise(SIGTRAP)
    #else
        #define debugBreak()
    #endif
#else
    #define debugBreak()
#endif

#define YGGDRASIL_USE_ASSERTS 0

#if !defined(NDEBUG) || YGGDRASIL_USE_ASSERTS
    #include "Yggdrasil/core/util/Log.h"

    FORCE_INLINE void logAssertionFailure(const char* expression, const char* file, int32_t line)
    {
        YGGDRASIL_CORE_CRITICAL("ASSERT: expression: {0}, file: {1}, line: {2}", expression, file, line);
    }

    FORCE_INLINE void logAssertionFailure(const char* msg, const char* expression, const char* file, int32_t line)
    {
        YGGDRASIL_CORE_CRITICAL("ASSERT: message: {0}, expression: {1},  file: {2}, line: {3}", msg, expression, file, line);
    }

    #define YGGDRASIL_ASSERT(x) {                                   \
        if(x) {}                                                    \
        else {                                                      \
            logAssertionFailure(#x, __FILE__, __LINE__);            \
            debugBreak();                                           \
        }                                                           \
    }
    
    #define YGGDRASIL_ASSERT_MSG(x, msg) {                          \
        if(x) {}                                                    \
        else {                                                      \
            logAssertionFailure(msg, #x, __FILE__, __LINE__);       \
            debugBreak();                                           \
        }                                                           \
    }
    
    #define YGGDRASIL_ASSERT_VALUE(x) {                             \
        if(x) {}                                                    \
        else {                                                      \
            logAssertionFailure(#x, __FILE__, __LINE__);            \
            debugBreak();                                           \
        }                                                           \
    }

    #define YGGDRASIL_ASSERT_VALUE_MSG(x, msg) {                    \
        if(x) {}                                                    \
        else {                                                      \
            logAssertionFailure(msg, #x, __FILE__, __LINE__);       \
            debugBreak();                                           \
        }                                                           \
    }
#else
    #define YGGDRASIL_ASSERT(x) x
    #define YGGDRASIL_ASSERT_MSG(x, msg) x
    #define YGGDRASIL_ASSERT_VALUE(x)
    #define YGGDRASIL_ASSERT_VALUE_MSG(x, message)
#endif
