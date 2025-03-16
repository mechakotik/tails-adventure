#include "item_box.h"
#include <cmath>
#include "character.h"
#include "error.h"
#include "save.h"
#include "tools.h"

void TA_ItemBox::load(TA_Point position, TA_Point velocity, int itemNumber, std::string itemName) {
    this->position = position;
    this->velocity = velocity;
    this->itemNumber = itemNumber;
    this->itemName = itemName;

    TA_Sprite::load("hud/items.png", 16, 16);
    TA_Sprite::setFrame(39);

    sound.load("sound/find_item.ogg", TA_SOUND_CHANNEL_SFX1);

    hitbox.setRectangle(TA_Point(8, 0), TA_Point(9, 16));
    updatePosition();
}

bool TA_ItemBox::update() {
    switch(state) {
        case STATE_IDLE:
            if(characterHasThisItem()) {
                return false;
            }
            updateIdle();
            break;
        case STATE_FALL:
            updateFall();
            break;
        case STATE_UNPACK:
            updateUnpack();
            break;
        case STATE_RAISE:
            updateRaise();
            break;
        case STATE_HOLD:
            if(!updateHold()) {
                return false;
            }
            break;
    }

    updatePosition();
    return true;
}

bool TA_ItemBox::characterHasThisItem() {
    long long itemMask = TA::save::getSaveParameter("item_mask");
    return (itemMask & (1ll << itemNumber)) != 0;
}

void TA_ItemBox::updateIdle() {
    if(objectSet->getLinks().character->isRemoteRobot()) {
        hitbox.setRectangle(TA_Point(2, 0), TA_Point(14, 17));
    } else {
        hitbox.setRectangle(TA_Point(8, 0), TA_Point(9, 17));
    }

    int flags = objectSet->checkCollision(hitbox);

    if((flags & (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP)) == 0) {
        state = STATE_FALL;
        return;
    }

    if((flags & TA_COLLISION_CHARACTER) != 0 && objectSet->getLinks().character->isOnGround()) {
        objectSet->getLinks().character->setUnpackState();
        sound.play();
        addItemToCharacter();
        state = STATE_UNPACK;
    }
}

void TA_ItemBox::updateFall() {
    velocity.y += gravity * TA::elapsedTime;
    velocity.y = std::min(velocity.y, maxFallSpeed);
    int flags = moveAndCollide(TA_Point(8, 0), TA_Point(9, 16), velocity * TA::elapsedTime);

    if(flags & TA_GROUND_COLLISION) {
        state = STATE_IDLE;
        velocity = {0, 0};
    }
}

bool TA_ItemBox::checkPawnCollision(TA_Polygon& hitbox) {
    int flags = objectSet->checkCollision(hitbox);
    return (flags & (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP)) != 0;
}

void TA_ItemBox::updateUnpack() {
    timer += TA::elapsedTime;
    if(timer > unpackTime) {
        state = STATE_RAISE;
        objectSet->getLinks().character->setRaiseState();
        timer = 0;

        TA_Point absoluteTextPosition = position + TA_Point(0, 16);
        objectSet->spawnObject<TA_ItemLabel>(objectSet->getLinks().camera->getRelative(absoluteTextPosition), itemName);

        return;
    }

    const double stepTime = unpackTime / 3;
    double stepTimer = std::fmod(timer, stepTime);

    if(stepTimer < stepTime / 3) {
        setAlpha(255 - 255 * (stepTimer / (stepTime / 5)));
    } else if(stepTimer < stepTime * 2 / 3) {
        if(timer > stepTime * 2) {
            setFrame(itemNumber);
        }
        setAlpha(255 * (stepTimer - stepTime * 2 / 3) / (stepTime / 3));
    } else {
        setAlpha(255);
    }
}

void TA_ItemBox::updateRaise() {
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

bool TA_ItemBox::updateHold() {
    timer += TA::elapsedTime;
    if(timer > holdTime) {
        objectSet->getLinks().character->setReleaseState();
        return false;
    }
    return true;
}

void TA_ItemBox::addItemToCharacter() {
    long long itemMask = TA::save::getSaveParameter("item_mask");
    itemMask |= (1ll << itemNumber);
    TA::save::setSaveParameter("item_mask", itemMask);

    if(itemNumber <= 19) {
        addItemToFirstFreeSlot();
    }
    if(itemNumber >= 29) {
        objectSet->addRingsToMaximum();
    }

    TA::save::writeToFile();
}

void TA_ItemBox::addItemToFirstFreeSlot() {
    int slot = getFirstFreeItemSlot();
    if(slot != -1) {
        TA::save::setSaveParameter(getItemSlotName(slot), itemNumber);
    }
}

int TA_ItemBox::getFirstFreeItemSlot() {
    int slot = 0;
    while(slot <= 3 && TA::save::getSaveParameter(getItemSlotName(slot)) != -1) {
        slot++;
    }
    if(slot == 4) {
        slot = -1;
    }
    return slot;
}

std::string TA_ItemBox::getItemSlotName(int number) {
    return "item_slot" + std::to_string(number);
}

int TA_ItemBox::getDrawPriority() {
    if(objectSet->getLinks().character->isRemoteRobot()) {
        return 0;
    }
    return 1;
}

void TA_ItemLabel::load(TA_Point position, std::string name) {
    font.loadFont("fonts/item.toml");
    position.x -= font.getTextWidth(name) / 2 - 8;
    this->position = position;
    this->name = name;
}

bool TA_ItemLabel::update() {
    timer += TA::elapsedTime;
    if(timer > showTime) {
        return false;
    }

    currentPosition = position;
    if(timer < appearTime) {
        double factor = timer / appearTime;
        font.setAlpha(255 * factor);
        currentPosition.x += shift * (1 - factor);
    } else if(timer > showTime - appearTime) {
        double factor = (timer - (showTime - appearTime)) / appearTime;
        font.setAlpha(255 * (1 - factor));
        currentPosition.x -= shift * factor;
    } else {
        font.setAlpha(255);
    }

    return true;
}

void TA_ItemLabel::draw() {
    font.drawText(currentPosition, name);
}
