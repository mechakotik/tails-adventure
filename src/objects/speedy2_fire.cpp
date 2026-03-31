#include "speedy2_fire.h"

void TA_Speedy2Fire::load(TA_Point position) {
    loadFromToml("objects/speedy2/fire.toml");
    setAnimation("fire");
    this->position = position;
    hitbox.setRectangle({8, 4}, {16, 24});
    updatePosition();
}

bool TA_Speedy2Fire::update() {
    static constexpr float flySpeed = 4;
    static constexpr float flyStartDelay = 50;
    static constexpr float glowInterval = 3;

    if(timer < flyStartDelay) {
        timer += TA::elapsedTime;

        glowTimer = std::fmod(glowTimer + TA::elapsedTime, glowInterval * 4);
        if(glowTimer < glowInterval) {
            setAlpha(0);
        } else if(glowTimer < glowInterval * 2) {
            setAlpha(static_cast<int>(255 * ((glowTimer - glowInterval) / glowInterval)));
        } else if(glowTimer < glowInterval * 3) {
            setAlpha(255);
        } else {
            setAlpha(static_cast<int>(255 - (255 * ((glowTimer - glowInterval * 3) / glowInterval))));
        }

        return true;
    }

    setAlpha(255);
    collisionType = TA_COLLISION_DAMAGE;
    position.x += flySpeed * TA::elapsedTime;
    updatePosition();

    int flags = objectSet->checkCollision(hitbox);
    return (flags & (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP)) == 0;
}
