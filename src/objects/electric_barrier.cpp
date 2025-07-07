#include "electric_barrier.h"
#include "tilemap.h"

void TA_ElectricBarrier::load(int top, int left, int bottom, int right, TA_Point switchPosition) {
    this->top = top;
    this->left = left;
    this->bottom = bottom;
    this->right = right;

    loadFromToml("objects/electric_barrier.toml");
    setAnimation("idle");
    hitbox.setRectangle({left * 16, top * 16}, {(right + 1) * 16, (bottom + 1) * 16});
    collisionType = TA_COLLISION_SOLID;

    switchSprite.loadFromToml("objects/electric_barrier_switch.toml");
    switchSprite.setPosition(switchPosition);
    switchSprite.setCamera(objectSet->getLinks().camera);
    switchHitbox.setRectangle({0, 0}, {8, 8});
    switchHitbox.setPosition(switchPosition);
    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
}

bool TA_ElectricBarrier::update() {
    if((objectSet->checkCollision(switchHitbox) & TA_COLLISION_CHARACTER) != 0) {
        switchSound.play();
        return false;
    }

    return true;
}

void TA_ElectricBarrier::draw() {
    forceUpdateAnimation();
    setUpdateAnimation(false);
    for(int tx = left; tx <= right; tx++) {
        for(int ty = top; ty <= bottom; ty++) {
            TA_Sprite::setPosition(tx * 16, ty * 16);
            TA_Sprite::draw();
        }
    }

    switchSprite.draw();
}
