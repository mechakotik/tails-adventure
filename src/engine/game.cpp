#include "game.h"
#include <chrono>
#include "SDL3_mixer/SDL_mixer.h"
#include "error.h"
#include "gamepad.h"
#include "keyboard.h"
#include "resource_manager.h"
#include "save.h"
#include "sound.h"
#include "tools.h"
#include "touchscreen.h"

TA_Game::TA_Game() {
    TA::save::load();
    initSDL();
    createWindow();
    TA::random::init(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    TA::gamepad::init();
    TA::resmgr::load();

    font.loadFont("fonts/pause_menu.toml");

    startTime = std::chrono::high_resolution_clock::now();
    screenStateMachine.init();
}

void TA_Game::initSDL() {
    if(!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD |
                 SDL_INIT_EVENTS | SDL_INIT_SENSOR)) {
        TA::handleSDLError("%s", "SDL init failed");
    }
    if(Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) {
        TA::handleSDLError("%s", "SDL_mixer init failed");
    }
    SDL_AudioSpec audioSpec;
    audioSpec.channels = TA_SOUND_CHANNEL_MAX;
    audioSpec.format = MIX_DEFAULT_FORMAT;
    audioSpec.freq = 44100;
    if(!Mix_OpenAudio(0, &audioSpec)) {
        TA::handleSDLError("%s", "Mix_OpenAudio failed");
    }
    SDL_HideCursor();
}

void TA_Game::createWindow() {
    TA::window = SDL_CreateWindow("Tails Adventure", defaultWindowWidth, defaultWindowHeight, SDL_WINDOW_FULLSCREEN);
    if(TA::window == nullptr) {
        TA::handleSDLError("%s", "failed to create window");
    }

    TA::renderer = SDL_CreateRenderer(TA::window, NULL);
    if(TA::renderer == nullptr) {
        TA::handleSDLError("%s", "failed to create renderer");
    }

    updateWindowSize();
    SDL_SetRenderDrawBlendMode(TA::renderer, SDL_BLENDMODE_BLEND);
    int vsync = TA::save::getParameter("vsync");
    SDL_SetRenderVSync(TA::renderer, (vsync == 2 ? -1 : vsync));
}

void TA_Game::toggleFullscreen() {
    fullscreen = !fullscreen;
    SDL_SetWindowFullscreen(TA::window, fullscreen);
    updateWindowSize();
}

void TA_Game::updateWindowSize() {
    int baseHeight = (TA::getBaseHeight(TA::save::getParameter("base_height")));
    float pixelAR = (TA::save::getParameter("pixel_ar") == 0 ? 1 : float(7) / 8);

    if(!fullscreen) {
        int factor = TA::save::getParameter("window_size");
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
        targetTexture = SDL_CreateTexture(
            TA::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, targetWidth, targetHeight);
    }

    SDL_SetTextureScaleMode(
        targetTexture, TA::save::getParameter("scale_mode") ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);
}

bool TA_Game::process() {
    updateWindowSize();

    TA::touchscreen::update();
    TA::keyboard::update();
    TA::gamepad::update();
    TA::sound::update();
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_EVENT_QUIT) {
            return false;
        }
        if(event.type == SDL_EVENT_FINGER_DOWN || event.type == SDL_EVENT_FINGER_MOTION ||
            event.type == SDL_EVENT_FINGER_UP) {
            TA::touchscreen::handleEvent(event.tfinger);
        } else if(event.type == SDL_EVENT_GAMEPAD_ADDED || event.type == SDL_EVENT_GAMEPAD_REMOVED) {
            TA::gamepad::handleEvent(event.gdevice);
        }
    }

    if(TA::keyboard::isScancodePressed(SDL_SCANCODE_RALT) && TA::keyboard::isScancodePressed(SDL_SCANCODE_RETURN) &&
        (TA::keyboard::isScancodeJustPressed(SDL_SCANCODE_RALT) ||
            TA::keyboard::isScancodeJustPressed(SDL_SCANCODE_RETURN))) {
        toggleFullscreen();
    }
    if(screenStateMachine.isQuitNeeded()) {
        return false;
    }
    return true;
}

void TA_Game::update() {
    currentTime = std::chrono::high_resolution_clock::now();
    TA::elapsedTime =
        static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - startTime).count()) /
        1e9F * 60;

    TA::elapsedTime = std::min(TA::elapsedTime, maxElapsedTime);
    // TA::elapsedTime /= 10;
    startTime = currentTime;

    SDL_SetRenderTarget(TA::renderer, targetTexture);
    SDL_SetRenderDrawColor(TA::renderer, 0, 0, 0, 255);
    SDL_RenderClear(TA::renderer);

    if(screenStateMachine.update()) {
        startTime = std::chrono::high_resolution_clock::now();
    }

    if(TA::save::getParameter("frame_time")) {
        int frameTime = static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(
            (std::chrono::high_resolution_clock::now() - startTime))
                .count());
        frameTimeSum += frameTime;
        frame += 1;
        if(frame % 60 == 0) {
            prevFrameTime = frameTimeSum / 60;
            frame = frameTimeSum = 0;
        }
        font.drawText(TA_Point(TA::screenWidth - 36, 24), std::to_string(prevFrameTime));
    }

    SDL_SetRenderTarget(TA::renderer, nullptr);
    SDL_SetRenderDrawColor(TA::renderer, 0, 0, 0, 255);
    SDL_RenderClear(TA::renderer);

    SDL_FRect srcRect{0, 0, (float)TA::screenWidth * TA::scaleFactor, (float)TA::screenHeight * TA::scaleFactor};
    SDL_FRect dstRect{0, 0, (float)windowWidth, (float)windowHeight};
    SDL_RenderTexture(TA::renderer, targetTexture, &srcRect, &dstRect);
    SDL_RenderPresent(TA::renderer);
}

TA_Game::~TA_Game() {
    TA::save::writeToFile();
    TA::gamepad::quit();
    TA::resmgr::quit();

    SDL_DestroyTexture(targetTexture);
    SDL_DestroyRenderer(TA::renderer);
    SDL_DestroyWindow(TA::window);

    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}
