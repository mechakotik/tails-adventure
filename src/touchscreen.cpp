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
    if(event.type == SDL_FINGERDOWN || event.type == SDL_FINGERMOTION) {
        if(event.type == SDL_FINGERDOWN) {
            justPressedFingers.insert(event.fingerId);
        }
        currentFingers[event.fingerId] = {event.x * TA::screenWidth, event.y * TA::screenHeight};
    }
    else {
        currentFingers.erase(event.fingerId);
    }
}

void TA_Button::update()
{
    if(mode) {
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
    else {
        pressed = hold = false;
        for(auto finger : TA::touchscreen::currentFingers) {
            TA_Point point = finger.second;
            if(inside(point)) {
                touchPosition = point;
                if(!justCreated && !TA::touchscreen::justPressedFingers.count(finger.first)) {
                    hold = true;
                }
                pressed = true;
            }
        }
    }
    justCreated = false;
}
