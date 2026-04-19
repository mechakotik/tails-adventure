#include "speedy2.h"
#include "defs.h"
#include "explosion.h"
#include "save.h"
#include "speedy2_electro.h"
#include "speedy2_fire.h"
#include "tools.h"

namespace {
    bool isComplete() {
        int64_t bossMask = TA::save::getSaveParameter("boss_mask");
        return (bossMask & TA_BOSS_SPEEDY2) != 0;
    }
} // namespace

void TA_Speedy2::load(TA_Point origin) {
    if(isComplete()) {
        return;
    }

    loadFromToml("objects/speedy2/speedy2.toml");
    flashSprite.loadFromToml("objects/speedy2/flash.toml");
    flashSprite.setCamera(objectSet->getLinks().camera);

    hitbox.setRectangle({7, 9}, {24, 40});
    this->origin = origin;
    position = origin;

    for(auto& seg : timeline) {
        seg.src += origin;
        seg.dst += origin;
        float dist = seg.src.getDistance(seg.dst);
        if(seg.action == TimelineAction::MOVE) {
            if(!TA::equal(dist, 0)) {
                seg.time = dist / seg.speed;
            } else {
                seg.time = 0.01F;
            }
        }
    }

    initSegment();
    updatePosition();

    waitSound.load("sound/boss_wait.ogg", TA_SOUND_CHANNEL_SFX2);
    electroChargeSound.load("sound/speedy2_electro_charge.ogg", TA_SOUND_CHANNEL_SFX2);
    electroReleaseSound.load("sound/speedy2_electro_release.ogg", TA_SOUND_CHANNEL_SFX2);
    fireSound.load("sound/speedy2_fire.ogg", TA_SOUND_CHANNEL_SFX2);
    hitSound.load("sound/hit.ogg", TA_SOUND_CHANNEL_SFX3);
    explosionSound.load("sound/explosion.ogg", TA_SOUND_CHANNEL_SFX3);
}

bool TA_Speedy2::update() {
    if(isComplete()) {
        return false;
    }

    if(health <= 0) {
        updateDestroyed();

        if(!isAnimated()) {
            int64_t bossMask = TA::save::getSaveParameter("boss_mask");
            bossMask |= TA_BOSS_SPEEDY2;
            TA::save::setSaveParameter("boss_mask", bossMask);

            TA::sound::playMusic("sound/bf.vgm");
            return false;
        }

        return isAnimated();
    }

    updateTimeline();
    updatePosition();
    return true;
}

void TA_Speedy2::updateTimeline() {
    if(timelinePos >= 8) {
        collisionType = TA_COLLISION_DAMAGE | TA_COLLISION_TARGET;
    }

    switch(timeline[timelinePos].action) {
        case TimelineAction::MOVE:
            updateMove();
            break;
        case TimelineAction::WAIT:
            updateWait();
            break;
        case TimelineAction::ELECTRO:
            updateElectro();
            break;
        case TimelineAction::FIRE:
            updateFire();
            break;
        default:
            break;
    }

    updatePosition();

    if(invincibleTimer > 0) {
        invincibleTimer -= TA::elapsedTime;
    } else if(timelinePos >= 8) {
        int flags = objectSet->checkCollision(hitbox);
        if((flags & TA_COLLISION_ATTACK) != 0) {
            hitSound.play();
            health--;
            if(health == 0) {
                explosionSound.play();
                setAnimation("destroyed");
                collisionType = TA_COLLISION_TRANSPARENT;
                timer = 0;
            } else {
                invincibleTimer = 60;
            }
        }
    }
}

void TA_Speedy2::initSegment() {
    if(timelinePos >= timeline.size()) {
        return;
    }
    const TimelineSegment& seg = timeline[timelinePos];

    switch(seg.action) {
        case TimelineAction::MOVE:
            position = seg.src;
            timer = 0;
            setAnimation(seg.animation);
            break;
        case TimelineAction::WAIT:
            timer = 0;
            setAnimation(seg.animation);
            break;
        case TimelineAction::PLAY_SOUND:
            if(seg.sound != nullptr) {
                seg.sound->play();
            }
            initNextSegment();
            break;
        case TimelineAction::START_BOSS_MUSIC:
            TA::sound::playMusic("sound/boss.vgm");
            initNextSegment();
            break;
        case TimelineAction::ELECTRO:
            electroChargeSound.play();
            setAnimation("electro_charge");
            objectSet->spawnObject<TA_Speedy2Electro>(position + TA_Point(-6, 13));
            timer = 0;
            break;
        case TimelineAction::FIRE:
            setAnimation("fire_charge");
            objectSet->spawnObject<TA_Speedy2Fire>(position + TA_Point(22, 10));
            timer = 0;
            break;
        default:
            break;
    }
}

void TA_Speedy2::initNextSegment() {
    timelinePos++;
    if(timelinePos >= timeline.size()) {
        timelinePos = 14;
    }
    initSegment();
}

void TA_Speedy2::updateMove() {
    const TimelineSegment& seg = timeline[timelinePos];

    timer += TA::elapsedTime;
    float normTimer = timer / seg.time;
    if(normTimer > 1.0F) {
        initNextSegment();
        return;
    }

    float tin = powf(normTimer, seg.easeIn);
    float tout = powf(1.0F - normTimer, seg.easeOut);
    float ease = tin / (tin + tout);
    position = seg.src + (seg.dst - seg.src) * ease;
}

void TA_Speedy2::updateWait() {
    timer += TA::elapsedTime;
    if(timer > timeline[timelinePos].time) {
        initNextSegment();
        return;
    }
}

void TA_Speedy2::updateElectro() {
    timer += TA::elapsedTime;
    if(timer > 50) {
        electroReleaseSound.play();
        initNextSegment();
        return;
    }
}

void TA_Speedy2::updateFire() {
    timer += TA::elapsedTime;
    if(timer > 50) {
        fireSound.play();
        initNextSegment();
        return;
    }
}

void TA_Speedy2::updateDestroyed() {
    soundTimer += TA::elapsedTime;
    while(soundTimer >= 40) {
        soundTimer -= 40;
        explosionSound.play();
    }
    timer += TA::elapsedTime;
    while(timer >= 6) {
        timer -= 6;
        int x = static_cast<int>(position.x) + static_cast<int>(TA::random::next() % 16);
        int y = static_cast<int>(position.y) + static_cast<int>(TA::random::next() % 24);
        objectSet->spawnObject<TA_Explosion>(TA_Point(x, y), 0, TA_EXPLOSION_NEUTRAL);
    }
}

void TA_Speedy2::draw() {
    TA_Object::draw();
    if(invincibleTimer >= 54 || (42 <= invincibleTimer && invincibleTimer <= 48)) {
        flashSprite.setPosition(position);
        flashSprite.setFrame(getCurrentFrame());
        flashSprite.draw();
    }
}
