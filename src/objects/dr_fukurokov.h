#include "object_set.h"

class TA_DrFukurokov : public TA_Object {
public:
    struct Properties {
        TA_Point startPosition;
        TA_Point controlPosition;
        TA_Point platformPosition;
        TA_Point exitBlockerPosition;
        float firstGunLeftX;
        float firstGunRightX;
        float firstGunY;
        float secondGunLeftX;
        float secondGunRightX;
        float secondGunY;
    };

    using TA_Object::TA_Object;
    void load(const Properties& properties);
    bool update() override;
    void draw() override;

private:
    enum class State { WAIT_CHARACTER, STEP, REMOVE_PLATFORM, CHARACTER_FALL, CONTROL, DEFEATED };

    struct Gun {
        TA_Sprite sprite;
        TA_Point position;
        float leftX;
        float rightX;
        bool flip = false;
        float timer = 0;
    };

    void updateWaitCharacter();
    void updateStep();
    void updateRemovePlatform();
    void updateCharacterFall();
    void updateControl();
    void updateGun(Gun& gun);

    State state = State::WAIT_CHARACTER;
    TA_Point startPosition, controlPosition;

    TA_Sound drFukurokovSound;
    TA_Sound quickFallSound;

    TA_Sprite characterMock;
    TA_Point mockPosition;
    TA_Point followPosition;
    float mockYSpeed = 0;

    TA_Sprite platformSprite;

    Gun firstGun;
    Gun secondGun;
};
