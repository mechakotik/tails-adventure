#ifndef TA_TOUCHSCREEN_H
#define TA_TOUCHSCREEN_H

#include <map>
#include <SDL.h>
#include "geometry.h"

namespace TA_TouchBackend {
    void processTouchEvent(SDL_TouchFingerEvent event);
    extern std::map<int, TA_Point> currentFingers;
}

struct TA_Button : TA_Polygon {
private:
    bool pressed = 0, hold = 0;

public:
    void update();
    bool isPressed() {return pressed;}
    bool isJustPressed() {return pressed && !hold;}
};

#endif // TA_TOUCHSCREEN_H
