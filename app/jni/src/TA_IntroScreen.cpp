#include "TA_IntroScreen.h"
#include "TA_Globals.h"

void TA_IntroScreen::init(int param)
{

}

TA_ScreenState TA_IntroScreen::update()
{
    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = gScreenWidth;
    rect.h = gScreenHeight;

    localTimer = (localTimer + gElapsedTime) % 4000000;
    int color = (localTimer < 2000000 ? 250 * (localTimer / 2000000.0) : 250 * ((4000000 - localTimer) / 2000000.0));
    SDL_SetRenderDrawColor(gRenderer, color, color, color, 0);
    SDL_RenderDrawRect(gRenderer, &rect);

    return {TA_SCREENSTATE_CURRENT, 0};
}

void TA_IntroScreen::quit()
{

}
