// Copyright 2021 Robert Ryan. See LICENCE.md.

#pragma once
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(_WIN64)
    #ifndef YGG_PLATFORM_WINDOWS
        #define YGG_PLATFORM_WINDOWS 1
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef STRICT
        #define STRICT
    #endif
#else
    #error "Unsupported Platform!"
#endif

#define YGG_USE_ASSERTS 1
#if YGG_USE_ASSERTS
    #include "Yggdrasil/Common/Util/Logger.h"
    __forceinline void LogAssertionFailure(const char* file, int32_t line, const char* expression)
    {
        YGG_CRITICAL("Assertion Failed! File: '{0}', Line: '{1}', Expression: '{2}'",
            file, line, expression);
    }
#define YGG_ASSERT(x) \
    { \
        if(x)   \
        {} else {\
        LogAssertionFailure(__FILE__, __LINE__, #x); \
        __debugbreak(); }\
    }
#else
    #define YGG_ASSERT(x)
#endif
