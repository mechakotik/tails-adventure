#ifndef TA_ERROR_H
#define TA_ERROR_H

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <cstdlib>
#include <cstdio>
#include "SDL3/SDL.h"

#define APP_NAME "com.mechakotik.tailsadventure"

namespace TA {
    template<typename... T>
    void handleError(const char* format, T... args)
    {
        #ifdef __ANDROID__
            __android_log_print(ANDROID_LOG_ERROR, APP_NAME, format, args...);
        #else
            std::printf(format, args...);
            std::printf("\n");
        #endif
        
        exit(0);
    }

    template<typename... T>
    void handleSDLError(const char* format, T... args)
    {
        #ifdef __ANDROID__
            __android_log_print(ANDROID_LOG_ERROR, APP_NAME, format, args...);
            __android_log_print(ANDROID_LOG_ERROR, APP_NAME, "%s", SDL_GetError());
        #else
            std::printf(format, args...);
            std::printf("\n%s\n", SDL_GetError());
        #endif
        
        exit(0);
    }

    template<typename... T>
    void printLog(const char* format, T... args)
    {
        #ifdef __ANDROID__
            __android_log_print(ANDROID_LOG_DEBUG, APP_NAME, format, args...);
        #else
            std::printf(format, args...);
            std::printf("\n");
        #endif
    }

    template<typename... T>
    void printWarning(const char* format, T... args)
    {
        #ifdef __ANDROID__
            __android_log_print(ANDROID_LOG_WARN, APP_NAME, format, args...);
        #else
            std::printf(format, args...);
            std::printf("\n");
        #endif
    }
}

#endif // TA_ERROR_H
