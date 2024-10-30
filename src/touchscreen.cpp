#include <map>
#include <set>
#include "touchscreen.h"
#include "tools.h"

namespace TA::touchscreen {
    struct Finger {
        TA_Point position, startPosition;
    };

    std::map<int, Finger> currentFingers;
    std::set<int> justPressedFingers;

    TA_Point scrollVector;
    int scrollId = -1;
}

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

        TA_Point position{event.x * TA::screenWidth, event.y * TA::screenHeight};
        if(!currentFingers.count(event.fingerID)) {
            currentFingers[event.fingerID].position = currentFingers[event.fingerID].startPosition = position;
        }

        if(scrollId == -1) {
            if(currentFingers[event.fingerID].position.getDistance(currentFingers[event.fingerID].startPosition) > 8) {
                scrollId = event.fingerID;
            }
        }
        else if(event.fingerID == scrollId) {
            scrollVector = currentFingers[event.fingerID].position - position;
        }

        currentFingers[event.fingerID].position = position;
    }
    else {
        if(event.fingerID == scrollId) {
            scrollId = -1;
        }
        currentFingers.erase(event.fingerID);
    }
}

bool TA::touchscreen::isScrolling()
{
    return scrollId != -1;
}

TA_Point TA::touchscreen::getScrollVector()
{
    return scrollVector;
}

void TA_OnscreenButton::update()
{
    if(fingerId == -1) {
        released = false;
        for(int id : TA::touchscreen::justPressedFingers) {
            TA_Point point = TA::touchscreen::currentFingers[id].position;
            if(inside(point) && id != TA::touchscreen::scrollId) {
                fingerId = id;
                break;
            }
        }
    }
    else {
        if(TA::touchscreen::scrollId == fingerId) {
            fingerId = -1;
            hold = released = false;
        }
        else if(!TA::touchscreen::currentFingers.count(fingerId)) {
            fingerId = -1;
            hold = false;
            released = true;
        }
        else if(!inside(TA::touchscreen::currentFingers[fingerId].position)) {
            fingerId = -1;
            hold = released = false;
        }
        else {
            hold = true;
        }
    }
}

void TA_OnscreenStick::update()
{
    if(fingerId == -1) {
        for(auto finger : TA::touchscreen::currentFingers) {
            TA_Point point = finger.second.position;
            if(inside(point)) {
                fingerId = finger.first;
                touchPosition = point;
            }
        }
    }
    else {
        if(TA::touchscreen::currentFingers.count(fingerId)) {
            touchPosition = TA::touchscreen::currentFingers[fingerId].position;
            hold = true;
        }
        else {
            fingerId = -1;
            hold = false;
        }
    }
}
