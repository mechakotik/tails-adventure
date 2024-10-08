#include "bridge.h"
#include "tools.h"
#include "particle.h"

void TA_Bridge::load(TA_Point newPosition, std::string filename, std::string newParticleFilename)
{
    TA_Sprite::load(filename, 16, 16);
    position = newPosition;
    hitbox.setRectangle(TA_Point(0, 0), TA_Point(16, 16));
    collisionHitbox.setRectangle(TA_Point(0, -0.01), TA_Point(16, 0));
    collisionHitbox.setPosition(position);
    particleFilename = newParticleFilename;
    breakSound.load("sound/break.ogg", TA_SOUND_CHANNEL_SFX2);
    updatePosition();
}

bool TA_Bridge::update()
{
    switch(state) {
        case TA_BRIDGE_STATE_IDLE:
            if((objectSet->checkCollision(collisionHitbox) & TA_COLLISION_CHARACTER) &&
                objectSet->getLinks().character->isOnGround()) {
                state = TA_BRIDGE_STATE_DELAY;
                timer = 0;
            }
            break;

        case TA_BRIDGE_STATE_DELAY:
            timer += TA::elapsedTime;
            if(timer > delayTime) {
                state = TA_BRIDGE_STATE_FALLING;
                TA_Sprite::setFrame(1);
                objectSet->spawnObject<TA_Particle>(particleFilename, position + TA_Point(0, 10), TA_Point(0, initialSpeed), TA_Point(0, grv));
                objectSet->spawnObject<TA_Particle>(particleFilename, position + TA_Point(10, 10), TA_Point(0, initialSpeed), TA_Point(0, grv), 4);
                timer = 0;
            }
            break;

        case TA_BRIDGE_STATE_FALLING:
            timer += TA::elapsedTime;
            TA_Sprite::setAlpha(255 - 255 * pow(timer / fallingTime, 6));
            if(timer > fallingTime / 2 && !particlesThrown) {
                objectSet->spawnObject<TA_Particle>(particleFilename, position, TA_Point(0, initialSpeed), TA_Point(0, grv));
                objectSet->spawnObject<TA_Particle>(particleFilename, position + TA_Point(10, 0), TA_Point(0, initialSpeed), TA_Point(0, grv), 4);
                particlesThrown = true;
            }
            if(!TA::sound::isPlaying(TA_SOUND_CHANNEL_SFX2)) {
                breakSound.play();
            }
            if(timer > fallingTime) {
                return false;
            }
            break;
    }

    updatePosition();
    return true;
}

int TA_Bridge::getCollisionType()
{
    if(state == TA_BRIDGE_STATE_FALLING) {
        return TA_COLLISION_TRANSPARENT;
    }
    return TA_COLLISION_HALF_SOLID;
}
