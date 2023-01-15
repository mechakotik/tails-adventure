#include "breakable_block.h"

void TA_BreakableBlock::load(TA_Point newPosition)
{
    TA_Sprite::load("maps/pf/pf_block.png");
    position = newPosition;
    setPosition(position);
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(15, 15));
    hitbox.setPosition(position);
}

bool TA_BreakableBlock::update()
{
    int flags;
    objectSet->checkCollision(hitbox, flags);
    if((flags & TA_COLLISION_EXPLOSION) != 0) {
        return false;
    }
    return true;
}
