#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
    #define KRAKEN_PLATFORM_WINDOWS 1
#elif __ANDROID__
    #define KRAKEN_PLATFORM_ANDROID 1
    #error "Android is not supported."
#elif __APPLE__
    #error "Apple is not supported."
    #define KRAKEN_PLATFORM_APPLE 1
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE && TARGET_IPHONE_SIMULATOR
        #error "Iphone is not supported!"
    #elif TARGET_OS_IPHONE
        #error "Iphone is not supported!"
    #elif TARGET_OS_MAC
        #define KRAKEN_PLATFORM_OSX 1
    #else
        #error "Unsupported Apple platform"
    #endif
#elif __linux__ || __linux || linux || __gnu_linux__
    #define KRAKEN_PLATFORM_LINUX 1
    #error "Linux is not supported."
#elif __unix__ || __unix
    #define KRAKEN_PLATFORM_UNKNOWN_UNIX 1
    #error "Unknown Unix platform."
#elif defined(_POSIX_VERSION) || __posix
    #define KRAKEN_PLATFORM_UNKNOWN_POSIX 1
    #error "Unsupported Posix platform."
#else
    #error "Unknown platform."
#endif

#define KRAKEN_UNUSED_VARIABLE(x) x

#if KRAKEN_PLATFORM_WINDOWS
    #define FORCE_INLINE __forceinline
    #define FORCE_NO_INLINE _declspec(noinline)
#else
    #define FORCE_INLINE inline
    #define FORCE_NO_INLINE
#endif

#if _MSC_VER
    #include <intrin.h>
    #define debugBreak() __debugbreak()
    #elif KRAKEN_PLATFORM_ANDROID || KRAKEN_PLATFORM_APPLE || KRAKEN_PLATFORM_LINUX
    #include <unistd.h>
    #if _POSIX_VERSION
        #include <signal.h>
        #define debugBreak() raise(SIGTRAP)
    #else
        #define debugBreak()
    #endif
#endif

#define KRAKEN_USE_ASSERTS 1

#if !defined(NDEBUG) || KRAKEN_USE_ASSERTS
    #include "KRAKEN/core/util/Log.h"

    FORCE_INLINE void logAssertionFailure(const char* expression, const char* file, int32_t line)
    {
        KRAKEN_CORE_CRITICAL(expression, file, line);
    }

    #define KRAKEN_ASSERT(x) {                                      \
        if(x) {}                                                    \
        else {                                                      \
            logAssertionFailure(#x, __FILE__, __LINE__);            \
            debugBreak();                                           \
        }                                                           \
    }
    
    #define KRAKEN_ASSERT_VALUE(x) {                                \
        if(x) {}                                                    \
        else {                                                      \
            logAssertionFailure(#x, __FILE__, __LINE__);            \
            debugBreak();                                           \
        }                                                           \
    }
#else
    #define KRAKEN_ASSERT(x, message) x
    #define KRAKEN_ASSERT_VALUE(x, message)
#endif
