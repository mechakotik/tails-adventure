#ifndef TA_TOUCHSCREEN_H
#define TA_TOUCHSCREEN_H

#include "SDL3/SDL.h"
#include "geometry.h"
#include "tools.h"

namespace TA { namespace touchscreen {
    void handleEvent(SDL_TouchFingerEvent event);
    void update();
} }

class TA_OnscreenButton : public TA_Polygon {
protected:
    bool pressed = false, hold = false;
    TA_Point touchPosition;

public:
    virtual void update();
    bool isPressed() {return pressed;}
    bool isJustPressed() {return pressed && !hold;}
    TA_Point getTouchPosition() {return touchPosition;}
};

class TA_OnscreenStick : public TA_OnscreenButton {
private:
    int fingerId = -1;

public:
    void update() override;
};

#endif // TA_TOUCHSCREEN_H
