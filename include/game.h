#ifndef TA_GAME_H
#define TA_GAME_H

#include <chrono>
#include "SDL3/SDL.h"
#include "screen_state_machine.h"
#include "font.h"

class TA_Game {
private:
    const int baseHeight = 144;
    const int defaultWindowWidth = 1024, defaultWindowHeight = 576;
    const double minWindowAspectRatio = 1.2, maxWindowAspectRatio = 2.4;
    const int soundFrequency = 44100, soundChunkSize = 256;
    const double maxElapsedTime = 4;

    void initSDL();
    void createWindow();
    void toggleFullscreen();
    void updateWindowSize();

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime, currentTime;
    TA_ScreenStateMachine screenStateMachine;

    SDL_Texture *targetTexture = nullptr;

    int windowWidth, windowHeight, targetWidth = 0, targetHeight = 0;
    bool vsync = false, fullscreen = true;

    TA_Font font;
    int frame = 0, frameTimeSum = 0, prevFrameTime = 0;

public:
    TA_Game();
    ~TA_Game();
    bool process();
    void update();
};

#endif //TA_GAME_H
