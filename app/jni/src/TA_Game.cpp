#include "TA_Game.h"
#include "TA_Error.h"

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
int gScreenWidth, gScreenHeight, gElapsedTime;

TA_Game::TA_Game()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        handleSDLError("SDL init failed");
    }
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        handleSDLError("SDL_image init failed");
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    gScreenWidth = baseHeight * pixelAspectRatio * displayMode.w / displayMode.h;
    gScreenHeight = baseHeight;

    gWindow = SDL_CreateWindow("Tails Adventure", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gScreenWidth, gScreenHeight, SDL_WINDOW_SHOWN);
    if(gWindow == nullptr) {
        handleSDLError("Failed to create window");
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(gRenderer == nullptr) {
        handleSDLError("Failed to create renderer");
    }

    startTime = std::chrono::high_resolution_clock::now();
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
