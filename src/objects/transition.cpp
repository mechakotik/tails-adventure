#include "transition.h"
#include "tools.h"
#include "character.h"
#include "save.h"

void TA_Transition::load(TA_Point topLeft, TA_Point bottomRight, std::string newLevelPath)
{
    hitbox.setRectangle(topLeft, bottomRight);
    screenState = TA_SCREENSTATE_GAME;
    levelPath = newLevelPath;
}

void TA_Transition::load(TA_Point topLeft, TA_Point bottomRight, int selection)
{
    hitbox.setRectangle(topLeft, bottomRight);
    screenState = TA_SCREENSTATE_MAP;
    this->selection = selection;
}

bool TA_Transition::update()
{
    int flags = objectSet->checkCollision(hitbox);
    if((flags & TA_COLLISION_CHARACTER) && (!objectSet->getLinks().character || !objectSet->getLinks().character->isRemoteRobot())) {
        if(screenState == TA_SCREENSTATE_GAME) {
            TA::levelPath = levelPath;
        }
        else {
            TA::save::setSaveParameter("map_selection", selection);
        }
        objectSet->setTransition(screenState);
    }
    return true;
}

void TA_Transition::draw()
{
    
}
