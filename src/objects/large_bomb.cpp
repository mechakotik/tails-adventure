#include "large_bomb.h"
#include "explosion.h"
#include "gamepad.h"
#include "object_set.h"
#include "tools.h"

void TA_LargeBomb::load(TA_Point position) {
    loadFromToml("objects/bomb.toml");
    setAnimation("large");

    this->position = position;
    hitbox.setRectangle(TA_Point(0, 3), TA_Point(16, 16));
    collisionType = TA_COLLISION_BOMB;
    updatePosition();

    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX3);
}

bool TA_LargeBomb::update() {
    velocity.y += gravity * TA::elapsedTime;
    auto [delta, flags] = objectSet->moveAndCollide(position, {1, 4}, {15, 16}, velocity * TA::elapsedTime,
        TA_COLLISION_SOLID | TA_COLLISION_SOLID_UP | TA_COLLISION_PUSHABLE);

    position += delta;
    updatePosition();

    if((flags & TA_GROUND_COLLISION) != 0) {
        velocity.y = 0;
    }

    if(!isAnimated() || (objectSet->checkCollision(hitbox) & TA_COLLISION_TARGET) != 0) {
        explode();
        return false;
    }
    return true;
}

void TA_LargeBomb::explode() {
    objectSet->spawnObject<TA_Explosion>(position, 0, TA_EXPLOSION_LARGE);

    for(int i = 1; i <= 3; i++) {
        TA_Point explosionPosition = position + TA_Point(static_cast<int>(TA::random::next() % 7) - 3,
                                                    static_cast<int>(TA::random::next() % 7) - 3);
        float explosionAngle =
            static_cast<float>(TA::random::next()) / static_cast<float>(TA::random::max()) * TA::pi * 2;
        TA_Point explosionVelocty = {
            std::cos(explosionAngle) * explosionSpeed, std::sin(explosionAngle) * explosionSpeed};

        explosionSound.play();
        objectSet->getLinks().camera->shake(32);
        TA::gamepad::rumble(0.75, 0, 20);
        objectSet->spawnObject<TA_Explosion>(explosionPosition, i * 16, TA_EXPLOSION_NEUTRAL, explosionVelocty);
    }
}
