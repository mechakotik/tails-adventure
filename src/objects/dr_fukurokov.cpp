#include "dr_fukurokov.h"
#include "sound.h"

void TA_DrFukurokov::load(TA_Point startPosition, TA_Point controlPosition, TA_Point platformPosition) {
    loadFromToml("objects/dr_fukurokov.toml");
    this->startPosition = startPosition;
    this->controlPosition = controlPosition;

    position = startPosition;
    setPosition(position);
    setAnimation("idle");

    // TODO: don't hardcode music path
    TA::sound::playMusic("sound/boss.vgm");

    drFukurokovSound.load("sound/dr_fukurokov.ogg", TA_SOUND_CHANNEL_SFX1);
    quickFallSound.load("sound/quick_fall.ogg", TA_SOUND_CHANNEL_SFX1);

    mockPosition = objectSet->getCharacterSpawnPoint();
    characterMock.loadFromToml("tails/tails.toml");
    characterMock.setPosition(mockPosition);
    characterMock.setAnimation("walk");
    characterMock.setCamera(objectSet->getLinks().camera);
    objectSet->getLinks().character->setHide(true);

    platformSprite.loadFromToml("objects/dr_fukurokov_platform.toml");
    platformSprite.setPosition(platformPosition);
    platformSprite.setCamera(objectSet->getLinks().camera);
}

bool TA_DrFukurokov::update() {
    switch(state) {
        case State::WAIT_CHARACTER:
            updateWaitCharacter();
            break;
        case State::STEP:
            updateStep();
            break;
        case State::REMOVE_PLATFORM:
            updateRemovePlatform();
            break;
        case State::CHARACTER_FALL:
            updateCharacterFall();
            break;
        case State::CONTROL:
            updateControl();
            break;
        default:
            break;
    }

    followPosition = mockPosition + TA_Point(22 - TA::screenWidth / 2, 26 - TA::screenHeight / 2);
    return true;
}

void TA_DrFukurokov::updateWaitCharacter() {
    float prevX = mockPosition.x;
    mockPosition.x += TA::elapsedTime * 0.5F;
    objectSet->getLinks().camera->setFollowPosition(&followPosition);

    if(prevX < position.x - 80 && mockPosition.x >= position.x - 80) {
        drFukurokovSound.play();
    }

    if(mockPosition.x > position.x - 41) {
        mockPosition.x = position.x - 41;
        characterMock.setAnimation("idle");
        setAnimation("step");
        state = State::STEP;
    }
    characterMock.setPosition(mockPosition);
}

void TA_DrFukurokov::updateStep() {
    position.x -= 0.5F * TA::elapsedTime;
    setPosition(position);

    if(!isAnimated()) {
        setAnimation("remove_platform");
        state = State::REMOVE_PLATFORM;
    }
}

void TA_DrFukurokov::updateRemovePlatform() {
    if(isAnimated()) {
        return;
    }

    platformSprite.setAlpha(0);
    setAnimation("idle");
    characterMock.setAnimation("uncontrolled_fall");
    quickFallSound.play();
    state = State::CHARACTER_FALL;
}

void TA_DrFukurokov::updateCharacterFall() {
    static constexpr float gravity = 0.125;
    static constexpr float maxYSpeed = 3;

    mockYSpeed += gravity * TA::elapsedTime;
    mockYSpeed = std::min(mockYSpeed, maxYSpeed);
    mockPosition.y += mockYSpeed * TA::elapsedTime;
    characterMock.setPosition(mockPosition);

    TA_Rect mockHitbox;
    mockHitbox.setRectangle({18, 12}, {30, 39});
    mockHitbox.setPosition(mockPosition);

    if((objectSet->checkCollision(mockHitbox) & TA_COLLISION_SOLID) != 0) {
        characterMock.setAlpha(0);
        objectSet->getLinks().character->setCharacterPosition(mockPosition);
        objectSet->getLinks().character->restoreFollowPosition();
        objectSet->getLinks().character->setHide(false);

        position = controlPosition;
        setPosition(position);
        hitbox.setRectangle({0, 16}, {32, 32});
        hitbox.setPosition(position);
        setAnimation("control");
        state = State::CONTROL;
    }
}

void TA_DrFukurokov::updateControl() {
    if((objectSet->checkCollision(hitbox) & TA_COLLISION_PUSHABLE) != 0) {
        TA::sound::playMusic("sound/cc.vgm");
        setAnimation("defeated");
        state = State::DEFEATED;
    }
}

void TA_DrFukurokov::draw() {
    TA_Sprite::draw();
    characterMock.draw();
    platformSprite.draw();
}
