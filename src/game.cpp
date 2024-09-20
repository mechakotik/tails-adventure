#include <chrono>
#include <thread>

#include "SDL3_image/SDL_image.h"
#include "SDL3_mixer/SDL_mixer.h"

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
    TA::save::load();

    initSDL();
    createWindow();

    //TA::eventLog::openWrite("event_log");

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
    TA::resmgr::preload();

    startTime = std::chrono::high_resolution_clock::now();
    screenStateMachine.init();
}

void TA_Game::initSDL()
{
    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS | SDL_INIT_SENSOR) < 0) {
        TA::handleSDLError("%s", "SDL init failed");
    }
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        TA::handleSDLError("%s", "SDL_image init failed");
    }

    if(Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) {
        TA::handleSDLError("%s", "SDL_mixer init failed");
    }
    SDL_AudioSpec audioSpec;
    audioSpec.channels = TA_SOUND_CHANNEL_MAX;
    audioSpec.format = MIX_DEFAULT_FORMAT;
    audioSpec.freq = 44100;
    if(Mix_OpenAudio(0, &audioSpec) == -1) {
        TA::handleSDLError("%s", "Mix_OpenAudio failed");
    }
    SDL_HideCursor();
}

void TA_Game::createWindow()
{
    TA::window = SDL_CreateWindow("Tails Adventure", defaultWindowWidth, defaultWindowHeight, SDL_WINDOW_FULLSCREEN);
    if(TA::window == nullptr) {
        TA::handleSDLError("%s", "Failed to create window");
    }

    TA::renderer = SDL_CreateRenderer(TA::window, NULL);
    if(TA::renderer == nullptr) {
        TA::handleSDLError("%s", "Failed to create renderer");
    }

    updateWindowSize();
    SDL_SetRenderDrawBlendMode(TA::renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderVSync(TA::renderer, TA::save::getParameter("vsync"));
}

void TA_Game::toggleFullscreen()
{
    fullscreen = !fullscreen;
    SDL_SetWindowFullscreen(TA::window, fullscreen);
    updateWindowSize();
}

void TA_Game::updateWindowSize()
{
    double pixelAR = (TA::save::getParameter("pixel_ar") == 0 ? 1 : double(7)/8);

    if(!fullscreen) {
        int factor = TA::save::getParameter("resolution");
        int neededWidth = baseHeight * 16 / 9 * factor;
        int neededHeight = baseHeight * factor;
        SDL_SetWindowSize(TA::window, neededWidth, neededHeight);
    }

    SDL_GetWindowSize(TA::window, &windowWidth, &windowHeight);
    TA::screenWidth = baseHeight * windowWidth / windowHeight * pixelAR;
    TA::screenHeight = baseHeight;
    TA::scaleFactor = (windowWidth + TA::screenWidth - 1) / TA::screenWidth;

    if(targetWidth < TA::screenWidth * TA::scaleFactor || targetHeight < TA::screenHeight * TA::scaleFactor) {
        targetWidth = std::max(targetWidth, TA::screenWidth * TA::scaleFactor);
        targetHeight = std::max(targetHeight, TA::screenHeight * TA::scaleFactor);

        if(targetTexture != nullptr) {
            SDL_DestroyTexture(targetTexture);
        }
        targetTexture = SDL_CreateTexture(TA::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, targetWidth, targetHeight);
        SDL_SetTextureScaleMode(targetTexture, SDL_SCALEMODE_NEAREST);
    }
}

bool TA_Game::process()
{
    updateWindowSize();

    TA::touchscreen::update();
    TA::keyboard::update();
    TA::gamepad::update();
    TA::sound::update();
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
        else if (event.type == SDL_EVENT_FINGER_DOWN || event.type == SDL_EVENT_FINGER_MOTION || event.type == SDL_EVENT_FINGER_UP) {
            TA::touchscreen::handleEvent(event.tfinger);
        }
        else if(event.type == SDL_EVENT_GAMEPAD_ADDED || event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            TA::gamepad::handleEvent(event.gdevice);
        }
    }

    if(TA::keyboard::isScancodePressed(SDL_SCANCODE_RALT) && TA::keyboard::isScancodePressed(SDL_SCANCODE_RETURN) &&
        (TA::keyboard::isScancodeJustPressed(SDL_SCANCODE_RALT) || TA::keyboard::isScancodeJustPressed(SDL_SCANCODE_RETURN))) {
        toggleFullscreen();
    }
    if(screenStateMachine.isQuitNeeded()) {
        return false;
    }
    return true;
}

void TA_Game::update()
{
    currentTime = std::chrono::high_resolution_clock::now();

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
        if(TA::eventLog::isWriting()) {
            timeConverter.a = TA::elapsedTime;
            TA::eventLog::write(timeConverter.b);
        }
    }

    //TA::printLog("FPS %f", 60 / TA::elapsedTime);
    TA::elapsedTime = std::min(TA::elapsedTime, maxElapsedTime);
    //TA::elapsedTime /= 10;
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

    SDL_FRect srcRect{0, 0, (float)TA::screenWidth * TA::scaleFactor, (float)TA::screenHeight * TA::scaleFactor};
    SDL_FRect dstRect{0, 0, (float)windowWidth, (float)windowHeight};
    SDL_RenderTexture(TA::renderer, targetTexture, &srcRect, &dstRect);
    SDL_RenderPresent(TA::renderer);

    int fpsLimit = TA::getFPSLimit();
    if(!vsync && fpsLimit != 0) {
        auto endTime = currentTime + std::chrono::nanoseconds(int(1e9 / fpsLimit));
        while(std::chrono::high_resolution_clock::now() < endTime) {}
    }
}

TA_Game::~TA_Game()
{
    TA::gamepad::quit();
    TA::resmgr::quit();
    TA::eventLog::quit();

    SDL_DestroyTexture(targetTexture);
    SDL_DestroyRenderer(TA::renderer);
    SDL_DestroyWindow(TA::window);
    
    SDL_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
}
