#include <SDL_image.h>
#include <SDL_mixer.h>

#include "game.h"
#include "error.h"
#include "sound.h"
#include "touchscreen.h"

SDL_Window *gWindow;
SDL_Renderer *gRenderer;
int gScreenWidth, gScreenHeight;
double gElapsedTime, gWidthMultiplier, gHeightMultiplier;

TA_Game::TA_Game()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        handleSDLError("SDL init failed");
    }
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        handleSDLError("SDL_image init failed");
    }

    if(Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) {
        handleSDLError("SDL_mixer init failed");
    }
    if(Mix_OpenAudio(soundFrequency, MIX_DEFAULT_FORMAT, TA_SOUND_CHANNEL_MAX, soundChunkSize) == -1) {
        handleSDLError("Mix_OpenAudio failed");
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    gScreenWidth = baseHeight * pixelAspectRatio * displayMode.w / displayMode.h;
    gScreenHeight = baseHeight;
    gWidthMultiplier = double(displayMode.w) / gScreenWidth;
    gHeightMultiplier = double(displayMode.h) / gScreenHeight;

    gWindow = SDL_CreateWindow("Tails Adventure", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, displayMode.w, displayMode.h, SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(gWindow == nullptr) {
        handleSDLError("Failed to create window");
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(gRenderer == nullptr) {
        handleSDLError("Failed to create renderer");
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    startTime = std::chrono::high_resolution_clock::now();
    screenStateMachine.init();
}

bool TA_Game::process()
{
    TA_TouchBackend::update();
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
        else if (event.type == SDL_FINGERDOWN || event.type == SDL_FINGERMOTION || event.type == SDL_FINGERUP) {
            TA_TouchBackend::processTouchEvent(event.tfinger);
        }
    }

    return true;
}

void TA_Game::update()
{
    currentTime = std::chrono::high_resolution_clock::now();
    gElapsedTime = (double)(std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime).count()) / 1e6 * 60;
    startTime = currentTime;

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    if(screenStateMachine.update()) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    SDL_RenderPresent(gRenderer);
}

TA_Game::~TA_Game()
{
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    SDL_Quit();
}
