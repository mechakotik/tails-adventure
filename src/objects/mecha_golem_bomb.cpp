#include "mecha_golem_bomb.h"
#include "explosion.h"

void TA_MechaGolemBomb::load(TA_Point position) {
    TA_Sprite::load("objects/mecha_golem/bomb.png");
    hitbox.setRectangle({4, 4}, {12, 28});
    this->position = position;
}

bool TA_MechaGolemBomb::update() {
    speed = std::min(maxSpeed, speed + gravity * TA::elapsedTime);
    position.y += speed * TA::elapsedTime;
    updatePosition();

    if(position.y >= 128) {
        objectSet->spawnObject<TA_Explosion>(position + TA_Point(0, 16));
        return false;
    }

    return true;
}
