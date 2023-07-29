#ifndef TA_ERROR_H
#define TA_ERROR_H

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <cstdio>
#include "SDL2/SDL.h"

#define APP_NAME "com.mechakotik.tailsadventure"

namespace TA {
    template<typename... T>
    void handleError(T... args)
    {
        #ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_ERROR, APP_NAME, args...);
        #else
        std::printf(args...);
        std::printf("\n");
        system("pause");
        #endif
        exit(0);
    }

    template<typename... T>
    void handleSDLError(T... args)
    {
        #ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_ERROR, APP_NAME, args...);
        __android_log_print(ANDROID_LOG_ERROR, APP_NAME, "%s", SDL_GetError());
        #else
        std::printf(args...);
        std::printf("\n%s\n", SDL_GetError());
        system("pause");
        #endif
        exit(0);
    }

    template<typename... T>
    void printLog(T... args)
    {
        #ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_DEBUG, APP_NAME, args...);
        #else
        std::printf(args...);
        std::printf("\n");
        #endif
    }

    template<typename... T>
    void printWarning(T... args)
    {
        #ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_WARN, APP_NAME, args...);
        #else
        std::printf(args...);
        std::printf("\n");
        #endif
    }
}

#endif // TA_ERROR_H
