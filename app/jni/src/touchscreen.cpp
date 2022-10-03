#include "touchscreen.h"
#include "globals.h"
#include "error.h"

namespace TA_TouchBackend {
    std::map<int, TA_Finger> currentFingers;
}

void TA_TouchBackend::processTouchEvent(SDL_TouchFingerEvent event)
{
    if(event.type == SDL_FINGERDOWN) {
        TA_Finger finger;
        finger.xpos = event.x * gScreenWidth;
        finger.ypos = event.y * gScreenHeight;
        finger.hold = 0;
        currentFingers[event.fingerId] = finger;
    }
    else if(event.type == SDL_FINGERMOTION) {
        currentFingers[event.fingerId].xpos = event.x * gScreenWidth;
        currentFingers[event.fingerId].ypos = event.y * gScreenHeight;
    }
    else {
        currentFingers.erase(event.fingerId);
    }
}

void TA_TouchBackend::updateHold()
{
    for(auto &finger : currentFingers) {
        finger.second.hold = 1;
    }
}
