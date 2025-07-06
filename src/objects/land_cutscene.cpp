#include "land_cutscene.h"
#include "save.h"
#include "sea_fox.h"
#include "tools.h"

void TA_LandCutscene::load(TA_Point position, float landY, int selection) {
    this->position = position;
    this->landY = landY;
    this->selection = selection;
    characterMock.loadFromToml("tails/tails.toml");
    characterMock.setCamera(objectSet->getLinks().camera);
    seaFoxMock.loadFromToml("tails/seafox.toml");
    seaFoxMock.setCamera(objectSet->getLinks().camera);
    seaFoxMock.setAnimation("idle");
    jumpSound.load("sound/jump.ogg", TA_SOUND_CHANNEL_SFX1);
}

bool TA_LandCutscene::update() {
    switch(state) {
        case State::IDLE:
            updateIdle();
            break;
        case State::CORRECT_X:
            updateCorrectX();
            break;
        case State::JUMP:
            updateJump();
            break;
        case State::WALK_AWAY:
            updateWalkAway();
            break;
    }

    return true;
}

void TA_LandCutscene::updateIdle() {
    float newY = objectSet->getLinks().seaFox->getPosition().y;
    if(characterY > position.y && newY <= position.y) {
        objectSet->getLinks().seaFox->setHide(true);
        seaFoxMock.setPosition({objectSet->getLinks().seaFox->getPosition().x, position.y});
        float cameraX = objectSet->getLinks().camera->getPosition().x;
        objectSet->getLinks().camera->setLockPosition({cameraX, position.y - 96});
        state = State::CORRECT_X;
    }
    characterY = newY;
}

void TA_LandCutscene::updateCorrectX() {
    TA_Point mockPosition = seaFoxMock.getPosition();

    if(mockPosition.x < position.x) {
        mockPosition.x += correctXSpeed * TA::elapsedTime;
        if(mockPosition.x >= position.x) {
            mockPosition.x = position.x;
            initJump();
        }
    } else {
        mockPosition.x -= correctXSpeed * TA::elapsedTime;
        if(mockPosition.x < position.x) {
            mockPosition.x = position.x;
            initJump();
        }
    }

    seaFoxMock.setPosition(mockPosition);
}

void TA_LandCutscene::initJump() {
    state = State::JUMP;
    velocity = {1, jumpSpeed};
    characterMock.setPosition(position - TA_Point(8, 8));
    characterMock.setAnimation("jump_up");
    seaFoxMock.setAnimation("empty");
    jumpSound.play();
}

void TA_LandCutscene::updateJump() {
    velocity.y += gravity * TA::elapsedTime;
    TA_Point characterPos = characterMock.getPosition();

    if(characterPos.y < landY && characterPos.y + velocity.y * TA::elapsedTime >= landY) {
        characterPos += velocity * TA::elapsedTime;
        characterPos.y = landY;
        characterMock.setAnimation("walk");
        state = State::WALK_AWAY;
    } else {
        characterPos += velocity * TA::elapsedTime;
        characterMock.setAnimation(velocity.y < 0 ? "jump_up" : "jump_down");
    }

    characterMock.setPosition(characterPos);
}

void TA_LandCutscene::updateWalkAway() {
    TA_Point characterPos = characterMock.getPosition();
    characterPos.x += TA::elapsedTime;
    characterMock.setPosition(characterPos);

    if(characterPos.x >= position.x + 96) {
        TA::save::setSaveParameter("map_selection", selection);
        TA::save::setSaveParameter("seafox", 0);
        objectSet->setTransition(TA_SCREENSTATE_MAP);
    }
}

void TA_LandCutscene::draw() {
    if(state != State::IDLE) {
        seaFoxMock.draw();
        if(state != State::CORRECT_X) {
            characterMock.draw();
        }
    }
}
