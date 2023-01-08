#ifndef TA_ERROR_H
#define TA_ERROR_H

#include <android/log.h>
#include "SDL.h"

#define APP_NAME "com.mechakotik.tailsadventure"

namespace TA {
    template<typename... T>
    void handleError(const char *format, T... args)
    {
        __android_log_print(ANDROID_LOG_ERROR, APP_NAME, format, args...);
        exit(0);
    }

    template<typename... T>
    void handleSDLError(T... args)
    {
        __android_log_print(ANDROID_LOG_ERROR, APP_NAME, args...);
        __android_log_print(ANDROID_LOG_ERROR, APP_NAME, "%s", SDL_GetError());
        exit(0);
    }

    template<typename... T>
    void printLog(T... args)
    {
        __android_log_print(ANDROID_LOG_DEBUG, APP_NAME, args...);
    }
}

#endif // TA_ERROR_H
