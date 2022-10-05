#include "touchscreen.h"
#include "globals.h"
#include "error.h"

namespace TA_TouchBackend {
    std::map<int, TA_Point> currentFingers;
}

void TA_TouchBackend::processTouchEvent(SDL_TouchFingerEvent event)
{
    if(event.type == SDL_FINGERDOWN || event.type == SDL_FINGERMOTION) {
        currentFingers[event.fingerId] = {event.x * gScreenWidth, event.y * gScreenHeight};
    }
    else {
        currentFingers.erase(event.fingerId);
    }
}

void TA_Button::update()
{
    bool pressedNow = 0;
    for(auto finger : TA_TouchBackend::currentFingers) {
        TA_Point point = finger.second;
        if(inside(point)) {
            if(pressed) {
                hold = 1;
            }
            pressed = pressedNow = 1;
        }
    }
    if(!pressedNow) {
        pressed = hold = 0;
    }
}
