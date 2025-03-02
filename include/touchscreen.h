#ifndef TA_TOUCHSCREEN_H
#define TA_TOUCHSCREEN_H

#include "SDL3/SDL.h"
#include "geometry.h"

namespace TA::touchscreen {
    void handleEvent(SDL_TouchFingerEvent event);
    void update();
    bool isScrolling();
    TA_Point getScrollVector();
}

class TA_OnscreenButton : public TA_Shape {
public:
    void update();
    bool isPressed() { return fingerId != -1; }
    bool isJustPressed() { return fingerId != -1 && !hold; }
    bool isReleased() { return released; }

private:
    bool hold = false, released = false;
    int fingerId = -1;
};

class TA_OnscreenStick : public TA_Shape {
public:
    void update();
    bool isPressed() { return fingerId != -1; }
    bool isJustPressed() { return fingerId != -1 && !hold; }
    TA_Point getTouchPosition() { return touchPosition; }

private:
    bool pressed = false, hold = false;
    int fingerId = -1;
    TA_Point touchPosition;
};

#endif // TA_TOUCHSCREEN_H
