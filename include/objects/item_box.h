#ifndef TA_ITEM_BOX_H
#define TA_ITEM_BOX_H

#include "object_set.h"
#include "font.h"

class TA_ItemBox : public TA_Object {
private:
    void updateIdle();
    void updateUnpack();
    void updateRaise();
    bool updateHold();
    bool characterHasThisItem();
    void drawItemName();
    void addItemToCharacter();

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

    TA_Sound sound;
    TA_Font font;
    std::string itemName;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, int itemNumber, std::string itemName);
    bool update();
    int getDrawPriority() override {return 1;}
};

class TA_ItemLabel : public TA_Object {
private:
    const double showTime = 100;
    const double appearTime = 10;
    const double shift = 32;

    std::string name;
    TA_Font font;
    TA_Point currentPosition;
    double timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, std::string name);
    bool update() override;
    void draw() override;
    int getDrawPriority() override {return 2;}
};

#endif // TA_ITEM_BOX_H
