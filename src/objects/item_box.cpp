#include "item_box.h"
#include "save.h"

void TA_ItemBox::load(TA_Point position, int itemNumber)
{
    this->position = position;
    this->itemNumber = itemNumber;

    TA_Sprite::load("hud/items.png", 16, 16);
    TA_Sprite::setFrame(39);

    hitbox.setRectangle(TA_Point(0, 0), TA_Point(16, 16));
    updatePosition();
}

bool TA_ItemBox::update()
{
    int itemMask = TA::save::getSaveParameter("item_mask");
    if(itemMask & (1 << itemNumber)) {
        return false;
    }
    if(objectSet->checkCollision(hitbox) & TA_COLLISION_CHARACTER) {
        itemMask |= (1 << itemNumber);
        return false;
    }
    return true;
}
