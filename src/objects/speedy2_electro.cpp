#include "speedy2_electro.h"
#include "explosion.h"
#include "tools.h"

void TA_Speedy2Electro::load(TA_Point position) {
    this->position = position;

    foregroundSprite.loadFromToml("objects/speedy2/electro.toml");
    foregroundSprite.setCamera(objectSet->getLinks().camera);
    foregroundSprite.setPosition(position);
    foregroundSprite.setAnimation("foreground");

    backgroundSprite.loadFromToml("objects/speedy2/electro.toml");
    backgroundSprite.setCamera(objectSet->getLinks().camera);
    backgroundSprite.setPosition(position);
    backgroundSprite.setAnimation("background");

    hitbox.setRectangle({2, 2}, {12, 12});
    updatePosition();
}

bool TA_Speedy2Electro::update() {
    static constexpr float flySpeed = 4;
    static constexpr float flyStartDelay = 50;

    if(timer < flyStartDelay) {
        timer += TA::elapsedTime;
        return true;
    }

    collisionType = TA_COLLISION_DAMAGE;
    position.x -= flySpeed * TA::elapsedTime;
    updatePosition();

    int flags = objectSet->checkCollision(hitbox);
    if((flags & (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP | TA_COLLISION_CHARACTER)) != 0) {
        bool hit = (flags & TA_COLLISION_CHARACTER) != 0;
        objectSet->spawnObject<TA_Explosion>(
            position - TA_Point(1, 1), 0, hit ? TA_EXPLOSION_ENEMY : TA_EXPLOSION_NEUTRAL);
        return false;
    }
    return true;
}

void TA_Speedy2Electro::draw() {
    static constexpr float glowInterval = 3;

    glowTimer = std::fmod(glowTimer + TA::elapsedTime, glowInterval * 4);
    if(glowTimer < glowInterval) {
        foregroundSprite.setAlpha(0);
    } else if(glowTimer < glowInterval * 2) {
        foregroundSprite.setAlpha(static_cast<int>(255 * ((glowTimer - glowInterval) / glowInterval)));
    } else if(glowTimer < glowInterval * 3) {
        foregroundSprite.setAlpha(255);
    } else {
        foregroundSprite.setAlpha(static_cast<int>(255 - (255 * ((glowTimer - glowInterval * 3) / glowInterval))));
    }

    backgroundSprite.draw();
    foregroundSprite.draw();
}

void TA_Speedy2Electro::updatePosition() {
    foregroundSprite.setPosition(position);
    backgroundSprite.setPosition(position);
    hitbox.setPosition(position);
}
