#include <map>
#include <set>
#include "touchscreen.h"
#include "error.h"

namespace TA { namespace touchscreen {
    std::map<int, TA_Point> currentFingers;
    std::set<int> justPressedFingers;
} }

void TA::touchscreen::update()
{
    justPressedFingers.clear();
}

void TA::touchscreen::handleEvent(SDL_TouchFingerEvent event)
{
    if(event.type == SDL_EVENT_FINGER_DOWN || event.type == SDL_EVENT_FINGER_MOTION) {
        if(event.type == SDL_EVENT_FINGER_DOWN) {
            justPressedFingers.insert(event.fingerID);
        }
        currentFingers[event.fingerID] = {event.x * TA::screenWidth, event.y * TA::screenHeight};
    }
    else {
        currentFingers.erase(event.fingerID);
    }
}

void TA_OnscreenButton::update()
{
    bool flag = false;
    for(auto finger : TA::touchscreen::currentFingers) {
        TA_Point point = finger.second;
        if(inside(point)) {
            touchPosition = point;
            if(TA::touchscreen::justPressedFingers.count(finger.first)) {
                pressed = true;
            }
            else if(pressed) {
                hold = true;
            }
            flag = true;
        }
    }
    if(!flag) {
        pressed = hold = false;
    }
}

void TA_OnscreenStick::update()
{
    if(fingerId == -1) {
        for(auto finger : TA::touchscreen::currentFingers) {
            TA_Point point = finger.second;
            if(inside(point)) {
                pressed = true;
                fingerId = finger.first;
                touchPosition = point;
            }
        }
    }
    else {
        if(TA::touchscreen::currentFingers.count(fingerId)) {
            touchPosition = TA::touchscreen::currentFingers[fingerId];
            hold = true;
        }
        else {
            fingerId = -1;
            pressed = hold = false;
        }
    }
}
