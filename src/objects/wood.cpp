#include "wood.h"
#include "dead_kukku.h"

void TA_Wood::load(TA_Point position) {
    loadFromToml("objects/wood.toml");
    setAnimation("wood");
    birdSprite.loadFromToml("objects/jumper.toml");
    birdSprite.setAnimation("jump");
    birdSprite.setCamera(objectSet->getLinks().camera);

    this->position = startPosition = position;
    hitbox.setRectangle({4, 4}, {12, 12});
    hitboxVector.resize(1);
    hitboxVector[0].hitbox.setRectangle({1, 1}, {15, 26});
}

bool TA_Wood::update() {
    if(idle) {
        updateIdle();
    } else {
        updateAttack();
    }

    updatePosition();

    if(bird) {
        birdSprite.setPosition(position - TA_Point(0, 31));
        hitboxVector[0].hitbox.setPosition(position - TA_Point(0, 31));
        hitboxVector[0].collisionType = TA_COLLISION_DAMAGE;
    } else {
        hitboxVector[0].collisionType = TA_COLLISION_TRANSPARENT;
    }

    return true;
}

void TA_Wood::draw() {
    if(!idle) {
        TA_Object::draw();
        if(bird) {
            birdSprite.draw();
        }
    }
}

void TA_Wood::updateIdle() {
    float camRightX = objectSet->getLinks().camera->getPosition().x + TA::screenWidth;
    if(camRightX < startPosition.x && startPosition.x < camRightX + 64) {
        position = startPosition;
        ground = false;
        idle = false;
        bird = (TA::random::next() % 2 == 0);
    }
}

void TA_Wood::updateAttack() {
    if(ground) {
        velocity = {-1, 0};
    } else {
        velocity.y = std::min(maxYSpeed, velocity.y + gravity * TA::elapsedTime);
    }

    if(bird && (objectSet->checkCollision(hitboxVector[0].hitbox) & TA_COLLISION_ATTACK) != 0) {
        objectSet->spawnObject<TA_DeadKukku>(position - TA_Point(4, 32));
        bird = false;
    }

    int flags = moveAndCollide({0, 0}, {16, 16}, velocity * TA::elapsedTime, ground);
    ground = (flags & TA_GROUND_COLLISION) != 0;

    if(objectSet->getLinks().camera->getPosition().x > position.x + 16) {
        idle = true;
    }
}

bool TA_Wood::checkPawnCollision(TA_Rect& hitbox) {
    int flags = objectSet->checkCollision(hitbox);
    return (flags & TA_COLLISION_SOLID) != 0;
}
