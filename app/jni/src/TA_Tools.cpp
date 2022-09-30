#include <algorithm>
#include <SDL.h>
#include "TA_Tools.h"
#include "TA_Error.h"
#include "TA_Globals.h"

void drawScreenRect(int r, int g, int b, int a)
{
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = displayMode.w;
    rect.h = displayMode.h;

    a = std::max(a, 0);
    a = std::min(a, 255);
    SDL_SetRenderDrawColor(gRenderer, r, g, b, a);
    SDL_RenderFillRect(gRenderer, &rect);
}

void drawShadow(int factor)
{
    drawScreenRect(0, 0, 0, factor);
}
