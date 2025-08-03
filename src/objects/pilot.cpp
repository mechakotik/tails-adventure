#include "pilot.h"
#include "dead_kukku.h"
#include "sea_fox.h"
#include "tilemap.h"

bool TA_PilotSpawner::update() {
    timer += TA::elapsedTime;
    if(timer < cooldown) {
        return true;
    }
    if(objectSet->getCharacterPosition().x < static_cast<float>(TA::screenWidth) ||
        objectSet->getCharacterPosition().x >
            static_cast<float>(objectSet->getLinks().tilemap->getWidth() - TA::screenWidth)) {
        return true;
    }

    if(TA::random::next() % 2 == 0) {
        if(TA::random::next() % 2 == 0) {
            objectSet->spawnObject<TA_Pilot>(TA_Point(0, TA::random::next() % 64 - 32));
        } else {
            objectSet->spawnObject<TA_Pilot>(TA_Point(TA::random::next() % 16, TA::random::next() % 64 - 32));
            objectSet->spawnObject<TA_Pilot>(TA_Point(48 + TA::random::next() % 16, TA::random::next() % 64 - 32));
        }
    } else {
        if(TA::random::next() % 2 == 0) {
            objectSet->spawnObject<TA_DivingPilot>(TA_Point(0, TA::random::next() % 32 - 16));
        } else {
            objectSet->spawnObject<TA_DivingPilot>(TA_Point(TA::random::next() % 16, TA::random::next() % 32 - 16));
            objectSet->spawnObject<TA_DivingPilot>(
                TA_Point(48 + TA::random::next() % 16, TA::random::next() % 32 - 16));
        }
    }

    timer = 0;
    return true;
}

void TA_Pilot::load(TA_Point offset) {
    loadFromToml("objects/pilot.toml");
    setAnimation("fly");

    flip = objectSet->getLinks().seaFox->getFlip();
    setFlip(flip);

    if(flip) {
        position.x = objectSet->getLinks().camera->getPosition().x - static_cast<float>(getWidth()) - 8;
        offset.x *= -1;
    } else {
        position.x = objectSet->getLinks().camera->getPosition().x + static_cast<float>(TA::screenWidth) + 8;
    }

    position.y = objectSet->getCharacterPosition().y - 36;
    position += offset;
    hitbox.setRectangle({4, 2}, {20, 30});
    collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    updatePosition();
}

bool TA_Pilot::update() {
    ysp = std::max(minYSpeed, ysp - gravity * TA::elapsedTime);
    position.x += (flip ? 1.0F : -1.0F) * TA::elapsedTime;
    position.y += ysp * TA::elapsedTime;
    updatePosition();

    int flags = objectSet->checkCollision(hitbox);
    if((flags & TA_COLLISION_ATTACK) != 0 || destroy) {
        objectSet->spawnObject<TA_DeadKukku>(position);
        return false;
    }
    if((flags & TA_COLLISION_CHARACTER) != 0) {
        destroy = true;
    }

    return true;
}

void TA_DivingPilot::load(TA_Point offset) {
    loadFromToml("objects/pilot.toml");
    setAnimation("dive");

    flip = objectSet->getLinks().seaFox->getFlip();
    setFlip(flip);

    if(flip) {
        position.x = objectSet->getLinks().camera->getPosition().x - static_cast<float>(getWidth()) - 8;
        offset.x *= -1;
    } else {
        position.x = objectSet->getLinks().camera->getPosition().x + static_cast<float>(TA::screenWidth) + 8;
    }

    position.y = objectSet->getCharacterPosition().y - 96;
    position += offset;
    hitbox.setRectangle({4, 2}, {20, 30});
    collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    updatePosition();
}

bool TA_DivingPilot::update() {
    switch(state) {
        case State::DIVE:
            updateDive();
            break;
        case State::TURN_BACK:
            updateTurnBack();
            break;
        case State::FLY_BACK:
            updateFlyBack();
            break;
    }

    updatePosition();

    int flags = objectSet->checkCollision(hitbox);
    if((flags & TA_COLLISION_ATTACK) != 0 || destroy) {
        objectSet->spawnObject<TA_DeadKukku>(position);
        return false;
    }
    if((flags & TA_COLLISION_CHARACTER) != 0) {
        destroy = true;
    }

    return true;
}

void TA_DivingPilot::updateDive() {
    static constexpr float diveTime = 60;

    position += TA_Point(flip ? 0.5 : -0.5, 1) * TA::elapsedTime;
    timer += TA::elapsedTime;

    if(timer > diveTime) {
        timer = 0;
        state = State::TURN_BACK;
        setAnimation("turn_back");
    }
}

void TA_DivingPilot::updateTurnBack() {
    position += TA_Point(0, 1) * TA::elapsedTime;

    if(!isAnimated()) {
        state = State::FLY_BACK;
        setAnimation("fly_back");
    }
}

void TA_DivingPilot::updateFlyBack() {
    position += TA_Point(flip ? -4 : 4, 0) * TA::elapsedTime;
}
