#ifndef TA_TOUCHSCREEN_H
#define TA_TOUCHSCREEN_H

#include <map>
#include <SDL.h>

struct TA_Finger {
    double xpos, ypos;
    bool hold;
};

namespace TA_TouchBackend {
    void processTouchEvent(SDL_TouchFingerEvent event);
    void updateHold();
    extern std::map<int, TA_Finger> currentFingers;
}

#endif // TA_TOUCHSCREEN_H
