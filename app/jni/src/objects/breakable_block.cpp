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
        const std::string rockPath = "maps/pf/pf_rock.png";
        objectSet->spawnParticle(rockPath, position + TA_Point(2, 2), {-0.5, -2}, {0, grv});
        objectSet->spawnParticle(rockPath, position + TA_Point(8, 2), {0.5, -2}, {0, grv});
        objectSet->spawnParticle(rockPath, position + TA_Point(2, 8), {-0.5, -0.5}, {0, grv});
        objectSet->spawnParticle(rockPath, position + TA_Point(8, 8), {0.5, -0.5}, {0, grv});
        return false;
    }
    return true;
}
