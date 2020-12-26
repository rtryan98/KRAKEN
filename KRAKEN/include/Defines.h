#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
    #define KRAKEN_PLATFORM_WINDOWS 1
#elif __ANDROID__
    #define KRAKEN_PLATFORM_ANDROID 1
    #error "Android is not supported."
#elif __APPLE__
    #error "Apple is not supported."
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
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
