#include "object_set.h"

class TA_ItemBox : public TA_Object {
private:
    void updateIdle();
    void updateUnpack();
    void updateRaise();
    void updateHold();
    bool characterHasThisItem();

    const double unpackTime = 40;
    const double holdTime = 60;

    enum State {
        STATE_IDLE,
        STATE_UNPACK,
        STATE_RAISE,
        STATE_HOLD
    };

    State state = STATE_IDLE;
    int itemNumber = 0;
    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, int itemNumber);
    bool update();
    int getDrawPriority() override {return 1;}
};
