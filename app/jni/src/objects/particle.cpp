#include "particle.h"
#include "engine/tools.h"

void TA_Particle::load(std::string filename, TA_Point newPosition, TA_Point newVelocity, TA_Point newDelta, double newDelay)
{
    TA_Sprite::load(filename);
    position = newPosition;
    velocity = newVelocity;
    delta = newDelta;
    delay = newDelay;
    setPosition(position);
}

bool TA_Particle::update()
{
    if(delay < 0) {
        velocity = velocity + delta;
        position = position + velocity;
        setPosition(position);
        timer += TA::elapsedTime;
        if(timer >= 300) {
            return false;
        }
    }
    else {
        delay -= TA::elapsedTime;
    }
    return true;
}
