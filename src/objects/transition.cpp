#include "transition.h"
#include "character.h"
#include "save.h"
#include "tools.h"

void TA_Transition::load(TA_Point topLeft, TA_Point bottomRight, std::string levelPath) {
    hitbox.setRectangle(topLeft, bottomRight);
    screenState = TA_SCREENSTATE_GAME;
    this->levelPath = levelPath;
}

void TA_Transition::load(TA_Point topLeft, TA_Point bottomRight, int selection, bool seaFox, int unlock) {
    hitbox.setRectangle(topLeft, bottomRight);
    screenState = TA_SCREENSTATE_MAP;
    this->selection = selection;
    this->seaFox = seaFox;
    this->unlock = unlock;
}

bool TA_Transition::update() {
    int flags = objectSet->checkCollision(hitbox);
    if((flags & TA_COLLISION_CHARACTER) &&
        (!objectSet->getLinks().character || !objectSet->getLinks().character->isRemoteRobot())) {
        if(screenState == TA_SCREENSTATE_GAME) {
            TA::levelPath = levelPath;
        } else {
            TA::save::setSaveParameter("map_selection", selection);
            TA::save::setSaveParameter("seafox", seaFox);
            if(unlock != 0) {
                int64_t mask = TA::save::getSaveParameter("area_mask");
                if((mask & (1LL << unlock)) == 0) {
                    mask |= (1LL << unlock);
                    TA::save::setSaveParameter("area_mask", mask);
                    TA::save::setSaveParameter("last_unlocked", unlock);
                }
            }
        }
        objectSet->setTransition(screenState);
    }
    return true;
}

void TA_Transition::draw() {}
