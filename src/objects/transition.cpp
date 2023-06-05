#include "transition.h"
#include "tools.h"
#include "character.h"

void TA_Transition::load(TA_Point topLeft, TA_Point bottomRight, TA_ScreenState newScreenState, std::string newLevelPath)
{
    hitbox.setRectangle(topLeft, bottomRight);
    screenState = newScreenState;
    levelPath = newLevelPath;
}

bool TA_Transition::update()
{
    int flags = objectSet->checkCollision(hitbox);
    if((flags & TA_COLLISION_CHARACTER) && !objectSet->getLinks().character->isRemoteRobot()) {
        TA::levelPath = levelPath;
        objectSet->setTransition(screenState);
    }
    return true;
}

void TA_Transition::draw()
{

}
