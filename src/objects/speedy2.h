#ifndef TA_SPEEDY2_H
#define TA_SPEEDY2_H

#include "object_set.h"

class TA_Speedy2 : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point origin);
    bool update() override;
    void draw() override;

private:
    enum class TimelineAction : uint8_t {
        MOVE,
        WAIT,
        PLAY_SOUND,
        START_BOSS_MUSIC,
        ELECTRO,
        FIRE,
    };

    struct TimelineSegment {
        TimelineAction action = TimelineAction::MOVE;
        TA_Point src;
        TA_Point dst;
        std::string animation;
        TA_Sound* sound = nullptr;
        float speed = 1;
        float time = 0;
        float easeIn = 1;
        float easeOut = 1;
    };

    std::vector<TimelineSegment> timeline = {
        {.src = {208, 133}, .dst = {135, 133}, .animation = "fly_forward", .speed = 2.5, .easeOut = 1.5},
        {.src = {135, 133}, .dst = {135, 138}, .animation = "fly", .speed = 0.5},
        {.action = TimelineAction::PLAY_SOUND, .sound = &waitSound},
        {.src = {135, 136}, .dst = {135, 136}, .animation = "wait"},
        {.action = TimelineAction::WAIT, .animation = "wait", .time = 80},
        {.action = TimelineAction::START_BOSS_MUSIC},

        {.src = {135, 138}, .dst = {135, 133}, .animation = "fly", .speed = 0.5},
        {.src = {135, 133}, .dst = {202, 133}, .animation = "fly_back", .speed = 2.5, .easeIn = 1.5},
        {.src = {202, 133}, .dst = {202, 4}, .animation = "fly", .speed = 2.5},
        {.src = {202, 4}, .dst = {8, 4}, .animation = "fly_forward", .speed = 2.5},
        {.src = {8, 4}, .dst = {8, 133}, .animation = "fly", .speed = 2.5},
        {.src = {8, 133}, .dst = {135, 133}, .animation = "fly_back", .speed = 2.5, .easeOut = 1.5},
        {.action = TimelineAction::ELECTRO},
        {.action = TimelineAction::WAIT, .animation = "electro_release", .time = 20},

        {.src = {135, 133}, .dst = {202, 133}, .animation = "fly_back", .speed = 2.5, .easeIn = 1.5},
        {.src = {202, 133}, .dst = {202, 4}, .animation = "fly", .speed = 2.5},
        {.src = {202, 4}, .dst = {104, 4}, .animation = "fly_forward", .speed = 2.5},
        {.src = {104, 4}, .dst = {104, 133}, .animation = "fly", .speed = 2.5},
        {.src = {104, 133}, .dst = {202, 133}, .animation = "fly_back", .speed = 2.5},
        {.src = {202, 133}, .dst = {202, 68}, .animation = "fly", .speed = 2.5},
        {.src = {202, 68}, .dst = {135, 68}, .animation = "fly_forward", .speed = 2.5, .easeOut = 1.5},
        {.action = TimelineAction::ELECTRO},
        {.action = TimelineAction::WAIT, .animation = "electro_release", .time = 20},

        {.src = {135, 68}, .dst = {202, 68}, .animation = "fly_back", .speed = 2.5, .easeIn = 1.5},
        {.src = {202, 68}, .dst = {202, 133}, .animation = "fly", .speed = 2.5},
        {.src = {202, 133}, .dst = {8, 133}, .animation = "fly_forward", .speed = 2.5},
        {.src = {8, 133}, .dst = {8, 4}, .animation = "fly", .speed = 2.5},
        {.src = {8, 4}, .dst = {135, 4}, .animation = "fly_back", .speed = 2.5, .easeOut = 1.5},
        {.action = TimelineAction::ELECTRO},
        {.action = TimelineAction::WAIT, .animation = "electro_release", .time = 20},

        {.src = {135, 4}, .dst = {104, 4}, .animation = "fly_forward", .speed = 2.5, .easeIn = 1.5},
        {.src = {104, 4}, .dst = {104, 133}, .animation = "fly", .speed = 2.5},
        {.src = {104, 133}, .dst = {202, 133}, .animation = "fly_back", .speed = 2.5},
        {.src = {202, 133}, .dst = {202, 4}, .animation = "fly", .speed = 2.5},
        {.src = {202, 4}, .dst = {8, 4}, .animation = "fly_forward", .speed = 2.5, .easeOut = 1.5},
        {.action = TimelineAction::FIRE},
        {.action = TimelineAction::WAIT, .animation = "fire_release", .time = 20},

        {.src = {8, 4}, .dst = {8, 68}, .animation = "fly", .speed = 2.5, .easeIn = 1.5, .easeOut = 1.5},
        {.action = TimelineAction::FIRE},
        {.action = TimelineAction::WAIT, .animation = "fire_release", .time = 20},

        {.src = {8, 68}, .dst = {8, 133}, .animation = "fly", .speed = 2.5, .easeIn = 1.5, .easeOut = 1.5},
        {.action = TimelineAction::FIRE},
        {.action = TimelineAction::WAIT, .animation = "fire_release", .time = 20},

        {.src = {8, 133}, .dst = {8, 4}, .animation = "fly", .speed = 2.5, .easeIn = 1.5},
        {.src = {8, 4}, .dst = {202, 4}, .animation = "fly_back", .speed = 2.5},
        {.src = {202, 4}, .dst = {202, 133}, .animation = "fly", .speed = 2.5},
        {.src = {202, 133}, .dst = {135, 133}, .animation = "fly_forward", .speed = 2.5, .easeOut = 1.5},
        {.action = TimelineAction::ELECTRO},
        {.action = TimelineAction::WAIT, .animation = "electro_release", .time = 20},
    };

    void updateTimeline();
    void initSegment();
    void initNextSegment();
    void updateMove();
    void updateWait();
    void updateElectro();
    void updateFire();
    void updateDestroyed();

    TA_Sprite flashSprite;
    TA_Point origin;
    int timelinePos = 0;
    float timer = 0;
    float invincibleTimer = 0;
    float soundTimer = 0;
    int health = 12;

    TA_Sound waitSound;
    TA_Sound electroChargeSound;
    TA_Sound electroReleaseSound;
    TA_Sound fireSound;
    TA_Sound hitSound;
    TA_Sound explosionSound;
};

#endif
