#include "wind.h"
#include "character.h"

void TA_Wind::load(TA_Point topLeft, TA_Point bottomRight, TA_Point velocity)
{
    hitbox.setRectangle(topLeft, bottomRight);
    this->velocity = velocity;
}

bool TA_Wind::update()
{
    if(shouldBlow()) {
        objectSet->getLinks().character->setWindVelociy(velocity);
        timer += TA::elapsedTime;
        if(timer > leafSpawnTime) {
            spawnLeaf();
            timer = 0;
        }
    }
    else {
        timer = 0;
    }

    return true;
}

bool TA_Wind::shouldBlow()
{
    if((objectSet->checkCollision(hitbox) & TA_COLLISION_CHARACTER) == 0) {
        return false;
    }
    if(objectSet->getLinks().character->isRemoteRobot()) {
        return false;
    }
    return objectSet->getLinks().character->isFlying();
}

void TA_Wind::spawnLeaf()
{
    Side side = getRandomLeafSide();
    TA_Point leafPosition = getRandomPointOnSide(side);

    TA_Point leafVelocity;
    leafVelocity.x = (TA::equal(velocity.x, 0) ? 0 : TA::sign(velocity.x) * 12);
    leafVelocity.y = (TA::equal(velocity.y, 0) ? 0 : TA::sign(velocity.y) * 12);

    objectSet->spawnObject<TA_Leaf>(leafPosition, leafVelocity);
}

TA_Wind::Side TA_Wind::getRandomLeafSide()
{
    std::vector<Side> sides;
    if(velocity.y > 0.5) {
        sides.push_back(SIDE_UP);
    }
    if(velocity.y < -0.5) {
        sides.push_back(SIDE_DOWN);
    }
    if(velocity.x > 0.5) {
        sides.push_back(SIDE_LEFT);
    }
    if(velocity.x < -0.5) {
        sides.push_back(SIDE_RIGHT);
    }
    
    int pos = TA::random::next() % (int)sides.size();
    return sides[pos];
}

TA_Point TA_Wind::getRandomPointOnSide(Side side)
{
    int width = TA::screenWidth, height = TA::screenHeight;
    switch(side) {
        case SIDE_UP:
            return TA_Point(TA::random::next() % width, -16);
        case SIDE_DOWN:
            return TA_Point(TA::random::next() % width, height + 16);
        case SIDE_LEFT:
            return TA_Point(-16, TA::random::next() % height);
        case SIDE_RIGHT:
            return TA_Point(width + 16, TA::random::next() % height);
    }
    return TA_Point(0, 0);
}

void TA_StrongWind::load(TA_Point topLeft, TA_Point bottomRight)
{
    TA_Wind::load(topLeft, bottomRight, {0, -3});
}

bool TA_StrongWind::shouldBlow()
{
    if(objectSet->checkCollision(hitbox) & TA_COLLISION_CHARACTER) {
        blowing = true;
    }
    if(objectSet->getLinks().character->isOnGround() ||
            objectSet->getLinks().character->isOnCeiling() ||
            objectSet->getLinks().character->isClimbing()) {
        blowing = false;
    }

    if(objectSet->getLinks().character->isRemoteRobot()) {
        return false;
    }
    return blowing;
}

void TA_Leaf::load(TA_Point position, TA_Point velocity)
{
    TA_Sprite::load("objects/leaf.png", 8, 8);
    TA_Sprite::loadAnimationsFromFile("objects/leaf.xml");
    TA_Sprite::setAnimation("leaf");
    TA_Sprite::setCamera(nullptr);

    this->position = position;
    this->velocity = velocity;
    updatePosition();
}

bool TA_Leaf::update()
{
    position = position + velocity * TA::elapsedTime;
    updatePosition();

    timer += TA::elapsedTime;
    if(timer > existTime) {
        return false;
    }
    return true;
}
