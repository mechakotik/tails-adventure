#ifndef TA_TOUCHSCREEN_H
#define TA_TOUCHSCREEN_H

#include "SDL3/SDL.h"
#include "geometry.h"
#include "tools.h"

namespace TA { namespace touchscreen {
    void handleEvent(SDL_TouchFingerEvent event);
    void update();
    bool isScrolling();
    TA_Point getScrollVector();
}}

class TA_OnscreenButton : public TA_Polygon {
protected:
    bool hold = false, released = false;
    int fingerId = -1;

public:
    void update();
    bool isPressed() {return fingerId != -1;}
    bool isJustPressed() {return fingerId != -1 && !hold;}
    bool isReleased() {return released;}
};

class TA_OnscreenStick : public TA_Polygon {
private:
    bool pressed = false, hold = false;
    int fingerId = -1;
    TA_Point touchPosition;

public:
    void update();
    bool isPressed() {return pressed;}
    bool isJustPressed() {return pressed && !hold;}
    TA_Point getTouchPosition() {return touchPosition;}
};

#endif // TA_TOUCHSCREEN_H
