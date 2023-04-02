#include "bird_walker.h"
#include "engine/tools.h"

void TA_BirdWalker::load(double newFloorY)
{
    floorY = newFloorY;
}

bool TA_BirdWalker::update()
{
    switch(state) {
        case TA_BIRD_WALKER_STATE_IDLE: {
            if(objectSet->getCamera()->isLocked()) {
                TA_Polygon currentHitbox;
                TA_Point cameraPosition = objectSet->getCamera()->getPosition();

                currentHitbox.setRectangle(cameraPosition + TA_Point(-1, 0), cameraPosition + TA_Point(0, TA::screenHeight));
                hitboxVector.push_back({currentHitbox, TA_COLLISION_SOLID});

                currentHitbox.setRectangle(cameraPosition + TA_Point(0, -1), cameraPosition + TA_Point(TA::screenWidth, 0));
                hitboxVector.push_back({currentHitbox, TA_COLLISION_SOLID});

                currentHitbox.setRectangle(cameraPosition + TA_Point(TA::screenWidth, 0), cameraPosition + TA_Point(TA::screenWidth + 1, TA::screenHeight));
                hitboxVector.push_back({currentHitbox, TA_COLLISION_SOLID});

                state = TA_BIRD_WALKER_STATE_AIR;
            }

            break;
        }

        default:
            break;
    }

    return true;
}
