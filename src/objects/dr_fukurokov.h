#include "object_set.h"

class TA_DrFukurokov : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point startPosition, TA_Point controlPosition, TA_Point platformPosition);
    bool update() override;
    void draw() override;

private:
    enum class State { WAIT_CHARACTER, STEP, REMOVE_PLATFORM, CHARACTER_FALL, CONTROL, DEFEATED };

    void updateWaitCharacter();
    void updateStep();
    void updateRemovePlatform();
    void updateCharacterFall();
    void updateControl();

    State state = State::WAIT_CHARACTER;
    TA_Point startPosition, controlPosition;

    TA_Sound drFukurokovSound;
    TA_Sound quickFallSound;

    TA_Sprite characterMock;
    TA_Point mockPosition;
    TA_Point followPosition;
    float mockYSpeed = 0;

    TA_Sprite platformSprite;
};
