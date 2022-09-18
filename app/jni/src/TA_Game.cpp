#include "TA_Game.h"
#include "TA_Error.h"

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
int gScreenWidth, gScreenHeight, gElapsedTime;
double gWidthMultiplier, gHeightMultiplier;

TA_Game::TA_Game()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        handleSDLError("%s", "SDL init failed");
    }
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        handleSDLError("%s", "SDL_image init failed");
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    gScreenWidth = baseHeight * pixelAspectRatio * displayMode.w / displayMode.h;
    gScreenHeight = baseHeight;
    gWidthMultiplier = double(displayMode.w) / gScreenWidth;
    gHeightMultiplier = double(displayMode.h) / gScreenHeight;

    gWindow = SDL_CreateWindow("Tails Adventure", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, displayMode.w, displayMode.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(gWindow == nullptr) {
        handleSDLError("%s", "Failed to create window");
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(gRenderer == nullptr) {
        handleSDLError("%s", "Failed to create renderer");
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    startTime = std::chrono::high_resolution_clock::now();
    screenStateMachine.init();
}

bool TA_Game::process()
{
    SDL_PollEvent(&event);
    return event.type != SDL_QUIT;
}

void TA_Game::update()
{
    currentTime = std::chrono::high_resolution_clock::now();
    gElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime).count();
    startTime = currentTime;

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);
    screenStateMachine.update();
    SDL_RenderPresent(gRenderer);
}

TA_Game::~TA_Game()
{
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}
