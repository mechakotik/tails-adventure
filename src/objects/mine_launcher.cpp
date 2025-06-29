#include "mine_launcher.h"
#include "enemy_mine.h"

void TA_MineLauncher::load(TA_Point position) {
    loadFromToml("objects/mine_launcher.toml");
    this->position = position;
    hitbox.setRectangle({0, 0}, {16, 16});
    collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    updatePosition();
}

bool TA_MineLauncher::update() {
    if(timer < cooldown) {
        timer += TA::elapsedTime;
        return true;
    }

    TA_Point characterPos = objectSet->getCharacterPosition();
    TA_Point delta(characterPos.x - position.x - 8, characterPos.y - position.y);
    if(std::abs(delta.x) <= 32 && 0 <= delta.y && delta.y <= 192) {
        objectSet->spawnObject<TA_EnemyMine>(position + TA_Point(1, 6), true);
        timer = 0;
    }

    return true;
}
