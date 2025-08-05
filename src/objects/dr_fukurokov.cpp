#include "dr_fukurokov.h"
#include "bullet.h"
#include "sound.h"
#include "tilemap.h"

void TA_DrFukurokov::load(const Properties& properties) {
    loadFromToml("objects/dr_fukurokov/dr_fukurokov.toml");
    this->startPosition = properties.startPosition;
    this->controlPosition = properties.controlPosition;

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

    platformSprite.loadFromToml("objects/dr_fukurokov/platform.toml");
    platformSprite.setPosition(properties.platformPosition);
    platformSprite.setCamera(objectSet->getLinks().camera);

    firstGun.sprite.loadFromToml("objects/dr_fukurokov/gun.toml");
    firstGun.sprite.setCamera(objectSet->getLinks().camera);
    firstGun.leftX = properties.firstGunLeftX;
    firstGun.rightX = properties.firstGunRightX;
    firstGun.position = {properties.firstGunLeftX, properties.firstGunY};
    secondGun.sprite.loadFromToml("objects/dr_fukurokov/gun.toml");
    secondGun.sprite.setCamera(objectSet->getLinks().camera);
    secondGun.leftX = properties.secondGunLeftX;
    secondGun.rightX = properties.secondGunRightX;
    secondGun.position = {properties.secondGunLeftX, properties.secondGunY};

    exitBlockerSprite.loadFromToml("objects/dr_fukurokov/exit_blocker.toml");
    exitBlockerSprite.setPosition(properties.exitBlockerPosition);
    exitBlockerSprite.setCamera(objectSet->getLinks().camera);
    hitbox.setRectangle({0, 0}, {exitBlockerSprite.getWidth(), exitBlockerSprite.getHeight()});
    hitbox.setPosition(properties.exitBlockerPosition);
    collisionType = TA_COLLISION_SOLID;
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

    if(state != State::DEFEATED) {
        updateGun(firstGun);
        updateGun(secondGun);
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
        controlHitbox.setRectangle({0, 16}, {32, 32});
        controlHitbox.setPosition(position);
        setAnimation("control");
        state = State::CONTROL;
    }
}

void TA_DrFukurokov::updateControl() {
    if((objectSet->checkCollision(controlHitbox) & TA_COLLISION_PUSHABLE) != 0) {
        TA::sound::playMusic("sound/cc.vgm");
        setAnimation("defeated");
        collisionType = TA_COLLISION_TRANSPARENT;
        state = State::DEFEATED;
    }
}

void TA_DrFukurokov::updateGun(Gun& gun) {
    static constexpr float cooldown = 120;

    if(gun.flip) {
        gun.position.x -= TA::elapsedTime;
        if(gun.position.x < gun.leftX) {
            gun.position.x = gun.leftX;
            gun.flip = false;
        }
    } else {
        gun.position.x += TA::elapsedTime;
        if(gun.position.x > gun.rightX) {
            gun.position.x = gun.rightX;
            gun.flip = true;
        }
    }

    gun.sprite.setPosition(gun.position);
    gun.timer += TA::elapsedTime;
    if(gun.timer > cooldown) {
        objectSet->spawnObject<TA_DrFukurokovLazer>(gun.position + TA_Point(2, 12), TA_Point(0, 3));
        gun.timer = 0;
    }
}

void TA_DrFukurokov::draw() {
    TA_Sprite::draw();
    characterMock.draw();
    platformSprite.draw();

    if(state != State::DEFEATED) {
        firstGun.sprite.draw();
        secondGun.sprite.draw();
        exitBlockerSprite.draw();
    }
}
