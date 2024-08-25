#include "grass_block.h"

void TA_GrassBlock::load(TA_Point position, std::string texture)
{
    TA_Sprite::load(texture, 16, 16);
    this->position = position;
    breakSound.load("sound/break.ogg", TA_SOUND_CHANNEL_SFX2);
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(16, 16));
    updatePosition();
}

bool TA_GrassBlock::update()
{
    if(objectSet->checkCollision(hitbox) & TA_COLLISION_NAPALM) {
        breakSound.play();
        return false;
    }
    return true;
}
