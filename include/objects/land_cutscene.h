#include "object_set.h"

class TA_LandCutscene : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position, float landY, int selection);
    bool update() override;
    void draw() override;

private:
    static constexpr float correctXSpeed = 0.7;
    static constexpr float jumpSpeed = -3.75;
    static constexpr float gravity = 0.125;

    enum class State { IDLE, CORRECT_X, JUMP, WALK_AWAY };

    void updateIdle();
    void updateCorrectX();
    void initJump();
    void updateJump();
    void updateWalkAway();

    State state = State::IDLE;
    float characterY = 10000;
    float landY = 0;
    int selection = 0;

    TA_Sprite characterMock;
    TA_Sprite seaFoxMock;
    TA_Sound jumpSound;

    TA_Point velocity;
};
