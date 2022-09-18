#include <algorithm>
#include <SDL.h>
#include "TA_Tools.h"
#include "TA_Error.h"
#include "TA_Globals.h"

void drawShadow(int factor)
{
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = displayMode.w;
    rect.h = displayMode.h;

    factor = std::max(factor, 0);
    factor = std::min(factor, 255);
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, factor);
    SDL_RenderFillRect(gRenderer, &rect);
}
