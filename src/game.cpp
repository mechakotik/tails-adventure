#include <chrono>
#include <thread>

#include "SDL_image.h"
#include "SDL_mixer.h"

#include "game.h"
#include "error.h"
#include "sound.h"
#include "touchscreen.h"
#include "tools.h"
#include "gamepad.h"
#include "resource_manager.h"
#include "keyboard.h"
#include "save.h"

TA_Game::TA_Game()
{
    initSDL();
    createWindow();

    TA::eventLog::openWrite("event_log");

    unsigned long long seed = std::chrono::steady_clock::now().time_since_epoch().count();
    union SeedConverter {
        unsigned long long a;
        long long b;
    } seedConverter;

    if(TA::eventLog::isReading()) {
        seedConverter.b = TA::eventLog::read();
        seed = seedConverter.a;
    }
    else {
        if(TA::eventLog::isWriting()) {
            seedConverter.a = seed;
            TA::eventLog::write(seedConverter.b);
        }
    }
    TA::random::init(seed);
    
    TA::gamepad::init();
    TA::keyboard::init();
    TA::save::load();
    TA::save::createSave("current_save");
    TA::save::setCurrentSave("current_save");

    startTime = std::chrono::high_resolution_clock::now();
    screenStateMachine.init();
}

void TA_Game::initSDL()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        TA::handleSDLError("SDL init failed");
    }
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        TA::handleSDLError("SDL_image init failed");
    }

    if(Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) {
        TA::handleSDLError("SDL_mixer init failed");
    }
    if(Mix_OpenAudio(soundFrequency, MIX_DEFAULT_FORMAT, TA_SOUND_CHANNEL_MAX, soundChunkSize) == -1) {
        TA::handleSDLError("Mix_OpenAudio failed");
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_ShowCursor(SDL_DISABLE);
}

void TA_Game::createWindow()
{
    int windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP;
    TA::window = SDL_CreateWindow("Tails Adventure", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, defaultWindowWidth, defaultWindowHeight, windowFlags);
    if(TA::window == nullptr) {
        TA::handleSDLError("Failed to create window");
    }

    TA::renderer = SDL_CreateRenderer(TA::window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(TA::renderer == nullptr) {
        TA::handleSDLError("Failed to create renderer");
    }

    updateWindowSize();
    SDL_SetRenderDrawBlendMode(TA::renderer, SDL_BLENDMODE_BLEND);
}

void TA_Game::toggleFullscreen()
{
    fullscreen = !fullscreen;
    SDL_SetWindowFullscreen(TA::window, (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));
    updateWindowSize();
}

void TA_Game::updateWindowSize()
{
    SDL_GetWindowSize(TA::window, &windowWidth, &windowHeight);
    double aspectRatio = double(windowWidth) / windowHeight;
    if(aspectRatio > maxWindowAspectRatio + 0.01 || aspectRatio < minWindowAspectRatio - 0.01) {
        int newWindowWidth = windowHeight * std::min(maxWindowAspectRatio, std::max(minWindowAspectRatio, aspectRatio));
        SDL_SetWindowSize(TA::window, newWindowWidth, windowHeight);
        updateWindowSize();
        return;
    }

    TA::screenWidth = baseHeight * windowWidth / windowHeight;
    TA::screenHeight = baseHeight;
    TA::widthMultiplier = TA::heightMultiplier = (windowWidth + TA::screenWidth - 1) / TA::screenWidth;

    if(targetTexture != nullptr) {
        SDL_DestroyTexture(targetTexture);
    }
    targetTexture = SDL_CreateTexture(TA::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TA::screenWidth * TA::widthMultiplier, TA::screenHeight * TA::heightMultiplier);
    SDL_SetTextureScaleMode(targetTexture, SDL_ScaleModeBest);
}

bool TA_Game::process()
{
    TA::touchscreen::update();
    TA::keyboard::update();
    TA::gamepad::update();
    TA::sound::update();
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
        else if (event.type == SDL_FINGERDOWN || event.type == SDL_FINGERMOTION || event.type == SDL_FINGERUP) {
            TA::touchscreen::handleEvent(event.tfinger);
        }
        else if(event.type == SDL_CONTROLLERDEVICEADDED || event.type == SDL_CONTROLLERDEVICEREMOVED) {
            TA::gamepad::handleEvent(event.cdevice);
        }
        else if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            updateWindowSize();
        }
    }

    if(TA::keyboard::isScancodePressed(SDL_SCANCODE_RALT) && TA::keyboard::isScancodePressed(SDL_SCANCODE_RETURN) &&
        (TA::keyboard::isScancodeJustPressed(SDL_SCANCODE_RALT) || TA::keyboard::isScancodeJustPressed(SDL_SCANCODE_RETURN))) {
        toggleFullscreen();
    }
    if(TA::keyboard::isScancodePressed(SDL_SCANCODE_ESCAPE)) {
        return false;
    }
    return true;
}

void TA_Game::update()
{
    currentTime = std::chrono::high_resolution_clock::now();
    if(firstFrame) {
        startTime = currentTime;
        firstFrame = false;
    }

    union TimeConverter {
        double a;
        long long b;
    } timeConverter;

    if(TA::eventLog::isReading()) {
        timeConverter.b = TA::eventLog::read();
        TA::elapsedTime = timeConverter.a;
    }
    else {
        TA::elapsedTime = (double)(std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime).count()) / 1e6 * 60;
        //TA::elapsedTime /= 10;
        if(TA::eventLog::isWriting()) {
            timeConverter.a = TA::elapsedTime;
            TA::eventLog::write(timeConverter.b);
        }
    }
    startTime = currentTime;

    SDL_SetRenderTarget(TA::renderer, targetTexture);
    SDL_SetRenderDrawColor(TA::renderer, 0, 0, 0, 255);
    SDL_RenderClear(TA::renderer);

    if(screenStateMachine.update()) {
        startTime = std::chrono::high_resolution_clock::now();
    }

    SDL_SetRenderTarget(TA::renderer, nullptr);
    SDL_SetRenderDrawColor(TA::renderer, 0, 0, 0, 255);
    SDL_RenderClear(TA::renderer);

    SDL_Rect srcRect{0, 0, TA::screenWidth * TA::widthMultiplier, TA::screenHeight * TA::heightMultiplier};
    SDL_Rect dstRect{0, 0, windowWidth, windowHeight};
    SDL_RenderCopy(TA::renderer, targetTexture, &srcRect, &dstRect);
    SDL_RenderPresent(TA::renderer);
}

TA_Game::~TA_Game()
{
    TA::gamepad::quit();
    TA::resmgr::quit();
    TA::eventLog::quit();
    SDL_DestroyRenderer(TA::renderer);
    SDL_DestroyWindow(TA::window);
    SDL_Quit();
}
