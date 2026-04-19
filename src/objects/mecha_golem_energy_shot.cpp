#include "mecha_golem_energy_shot.h"
#include "explosion.h"
#include "tools.h"

void TA_MechaGolemEnergyShot::load(TA_Point position) {
    this->position = position;

    foregroundSprite.loadFromToml("objects/mecha_golem/energy_shot.toml");
    foregroundSprite.setCamera(objectSet->getLinks().camera);
    foregroundSprite.setPosition(position);
    foregroundSprite.setAnimation("foreground");

    backgroundSprite.loadFromToml("objects/mecha_golem/energy_shot.toml");
    backgroundSprite.setCamera(objectSet->getLinks().camera);
    backgroundSprite.setPosition(position);
    backgroundSprite.setAnimation("background");

    hitbox.setRectangle({2, 2}, {12, 12});
    collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    updatePosition();

    TA_Point delta = objectSet->getCharacterPosition() - (position + TA_Point(7, 7));
    angle = std::atan2(delta.y, delta.x);
    angle = std::min(angle, TA::pi / 4);
    angle = std::max(angle, -TA::pi / 4);
    if(angle < 0) {
        angle = TA::pi * 2 + angle;
    }
}

bool TA_MechaGolemEnergyShot::update() {
    static constexpr float flySpeed = 0.8;
    static constexpr float turnSpeed = 0.02;

    TA_Point delta = objectSet->getCharacterPosition() - (position + TA_Point(7, 7));
    float neededAngle = std::atan2(delta.y, delta.x);
    if(neededAngle < 0) {
        neededAngle = TA::pi * 2 + neededAngle;
    }

    if(std::abs(angle - neededAngle) <= turnSpeed * TA::elapsedTime) {
        angle = neededAngle;
    } else {
        float positiveDiff = (neededAngle >= angle ? neededAngle - angle : (TA::pi * 2) - angle + neededAngle);
        float negativeDiff = (neededAngle <= angle ? angle - neededAngle : (TA::pi * 2) - neededAngle + angle);

        if(positiveDiff < negativeDiff) {
            angle = std::fmod(angle + (turnSpeed * TA::elapsedTime), TA::pi * 2);
        } else {
            angle = std::fmod(angle - (turnSpeed * TA::elapsedTime) + (TA::pi * 8), TA::pi * 2);
        }
    }

    position += TA_Point(std::cos(angle), std::sin(angle)) * flySpeed * TA::elapsedTime;
    updatePosition();

    int flags = objectSet->checkCollision(hitbox);
    if((flags & (TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP | TA_COLLISION_ATTACK | TA_COLLISION_CHARACTER)) != 0) {
        bool hit = (flags & TA_COLLISION_CHARACTER) != 0;
        objectSet->spawnObject<TA_Explosion>(
            position - TA_Point(1, 1), 0, hit ? TA_EXPLOSION_ENEMY : TA_EXPLOSION_NEUTRAL);
        return false;
    }
    return true;
}

void TA_MechaGolemEnergyShot::draw() {
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

void TA_MechaGolemEnergyShot::updatePosition() {
    foregroundSprite.setPosition(position);
    backgroundSprite.setPosition(position);
    hitbox.setPosition(position);
}
