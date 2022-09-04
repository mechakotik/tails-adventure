#include <android/log.h>
#include <SDL.h>
#include "TA_Error.h"

#define APP_NAME "com.mechakotik.tailsadventure"

void handleError(const char *error)
{
    __android_log_print(ANDROID_LOG_ERROR, APP_NAME, "%s", error);
    exit(0);
}

void handleSDLError(const char *error)
{
    __android_log_print(ANDROID_LOG_ERROR, APP_NAME, "%s", error, "\n", SDL_GetError());
    exit(0);
}

void printLog(const char *log)
{
    __android_log_print(ANDROID_LOG_DEBUG, APP_NAME, "%s", log);
}
