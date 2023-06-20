#include <cmath>
#include "item_box.h"
#include "save.h"
#include "tools.h"
#include "character.h"
#include "error.h"

void TA_ItemBox::load(TA_Point position, int itemNumber)
{
    this->position = position;
    this->itemNumber = itemNumber;

    TA_Sprite::load("hud/items.png", 16, 16);
    TA_Sprite::setFrame(39);

    hitbox.setRectangle(TA_Point(8, 0), TA_Point(9, 16));
    updatePosition();
}

bool TA_ItemBox::update()
{
    switch(state) {
        case STATE_IDLE:
            updateIdle();
            break;
        case STATE_UNPACK:
            updateUnpack();
            break;
        case STATE_RAISE:
            updateRaise();
            break;
        case STATE_HOLD:
            updateHold();
            break;
    }

    updatePosition();
    if(characterHasThisItem()) {
        return false;
    }
    return true;
}

bool TA_ItemBox::characterHasThisItem()
{
    long long itemMask = TA::save::getSaveParameter("item_mask");
    return (itemMask & (1ll << itemNumber)) != 0;
}

void TA_ItemBox::updateIdle()
{
    int flags = objectSet->checkCollision(hitbox);
    if((flags & TA_COLLISION_CHARACTER) != 0 && objectSet->getLinks().character->isOnGround()) {
        objectSet->getLinks().character->setUnpackState();
        state = STATE_UNPACK;
    }
}

void TA_ItemBox::updateUnpack()
{
    timer += TA::elapsedTime;
    if(timer > unpackTime) {
        state = STATE_RAISE;
        objectSet->getLinks().character->setRaiseState();
        timer = 0;
        return;
    }

    const double stepTime = unpackTime / 3;
    double stepTimer = std::fmod(timer, stepTime);
    
    if(stepTimer < stepTime / 3) {
        setAlpha(255 - 255 * (stepTimer / (stepTime / 5)));
    }
    else if(stepTimer < stepTime * 2 / 3) {
        if(timer > stepTime * 2) {
            setFrame(itemNumber);
        }
        setAlpha(255 * (stepTimer - stepTime * 2 / 3) / (stepTime / 3));
    }
    else {
        setAlpha(255);
    }
}

void TA_ItemBox::updateRaise()
{
    if(!objectSet->getLinks().character->isAnimated()) {
        state = STATE_HOLD;
        timer = 0;
        return;
    }

    int frame = objectSet->getLinks().character->getCurrentFrame();
    TA_Point addPosition;

    switch(frame) {
        case 78:
            addPosition = TA_Point(32, 23);
            break;
        case 74:
            addPosition = TA_Point(27, 13);
            break;
        case 75:
            addPosition = TA_Point(30, -1);
            break;
        case 76:
            addPosition = TA_Point(11, -6);
            break;
        case 77:
            addPosition = TA_Point(2, -6);
            break;
        default:
            break;
    }

    if(objectSet->getLinks().character->TA_Sprite::getFlip()) {
        addPosition.x = 32 - addPosition.x;
    }
    TA_Point characterPosition = objectSet->getLinks().character->TA_Sprite::getPosition();
    position = characterPosition + addPosition;
}

void TA_ItemBox::updateHold()
{
    timer += TA::elapsedTime;
    if(timer > holdTime) {
        long long itemMask = TA::save::getSaveParameter("item_mask");
        itemMask |= (1ll << itemNumber);
        TA::save::setSaveParameter("item_mask", itemMask);
        objectSet->getLinks().character->setAnimation("release");
    }
}
