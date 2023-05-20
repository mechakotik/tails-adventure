#include "bat_robot.h"
#include "tools.h"
#include "explosion.h"

void TA_BatRobot::load(TA_Point newPosition)
{
    TA_Sprite::load("objects/bat_robot.png", 24, 16);
    TA_Sprite::loadAnimationsFromFile("objects/bat_robot.xml");
    TA_Sprite::setAnimation("idle");

    position = newPosition;
    hitbox.setRectangle(TA_Point(5, 0), TA_Point(18, 14));
    updatePosition();
}

bool TA_BatRobot::checkPawnCollision(TA_Polygon &collisionHitbox)
{
    int flags = objectSet->checkCollision(collisionHitbox);
    return flags & TA_COLLISION_SOLID;
}

bool TA_BatRobot::update()
{
    switch (state) {
        case STATE_IDLE: {
            setAnimation("idle");
            TA_Point characterPosition = objectSet->getCharacterPosition();
            double centeredX = position.x + 12;
            if(abs(characterPosition.x - centeredX) <= 96 && characterPosition.y - position.y <= 64) {
                state = STATE_ACTIVE;
            }
            break;
        }

        case STATE_ACTIVE: {
            setAnimation("active");
            TA_Point characterPosition = objectSet->getCharacterPosition();
            timer += TA::elapsedTime;
            double centeredX = position.x + 12;
            if(timer >= cooldownTime && characterPosition.y - position.y <= 48 && abs(characterPosition.x - centeredX) <= 48) {
                position.y += 0.1;
                velocity = {TA::sign(characterPosition.x - position.x) * 0.5, 2 * speed};
                state = STATE_ATTACK;
            }
            else if(abs(characterPosition.x - position.x) >= 112 || characterPosition.y - position.y >= 80) {
                state = STATE_IDLE;
            }
            break;
        }

        case STATE_ATTACK: {
            setAnimation("attack");
            velocity.y += gravity;
            int flags = moveAndCollide(TA_Point(6, 0), TA_Point(17, 14), velocity);
            if(flags & TA_GROUND_COLLISION) {
                velocity.y = std::min(double(0), velocity.y);
            }
            if(flags & TA_WALL_COLLISION) {
                velocity.x = 0;
            }
            if(flags & TA_CEIL_COLLISION) {
                state = STATE_ACTIVE;
                timer = 0;
            }
            break;
        }
    }

    updatePosition();

    if(objectSet->checkCollision(hitbox) & TA_COLLISION_EXPLOSION) {
        objectSet->spawnObject<TA_Explosion>(position + TA_Point(4, 0));
        return false;
    }
    return true;
}
