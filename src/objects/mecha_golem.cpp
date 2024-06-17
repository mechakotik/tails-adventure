#include "mecha_golem.h"

void TA_MechaGolem::load()
{
    objectSet->getLinks().camera->setLockPosition({128, 0});
}

bool TA_MechaGolem::update()
{
    switch(state) {
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_P1_WAIT:
            updateP1Wait();
            break;
    }

    updateHitboxes();
    return true;
}

void TA_MechaGolem::updateIdle()
{
    if(objectSet->getLinks().camera->isLocked()) {
        state = STATE_P1_WAIT;
        objectSet->playBossMusic();
    }
}

void TA_MechaGolem::updateP1Wait()
{

}

void TA_MechaGolem::updateHitboxes()
{
    hitboxVector.clear();
    if(state != STATE_IDLE) {
        insertBorderHitboxes();
    }
}

void TA_MechaGolem::insertBorderHitboxes()
{
    TA_Polygon borderHitbox;
    borderHitbox.setRectangle(TA_Point(112, 0), TA_Point(128, 96));
    hitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});

    borderHitbox.setRectangle(TA_Point(128 + TA::screenWidth, 0), TA_Point(144 + TA::screenWidth, 96));
    hitboxVector.push_back({borderHitbox, TA_COLLISION_SOLID});
}

void TA_MechaGolem::draw()
{

}
