#ifndef TA_ITEM_BOX_H
#define TA_ITEM_BOX_H

#include "font.h"
#include "object_set.h"

class TA_ItemBox : public TA_Object {
private:
    void updateIdle();
    void updateFall();
    void updateUnpack();
    void updateRaise();
    bool updateHold();
    bool characterHasThisItem();
    void drawItemName();

    void addItemToCharacter();
    void addItemToFirstFreeSlot();
    int getFirstFreeItemSlot();
    std::string getItemSlotName(int number);

    const float unpackTime = 40;
    const float holdTime = 60;
    const float gravity = 0.125;
    const float maxFallSpeed = 3;

    enum State { STATE_IDLE, STATE_FALL, STATE_UNPACK, STATE_RAISE, STATE_HOLD };

    State state = STATE_IDLE;
    int itemNumber = 0;
    float timer = 0;

    TA_Sound sound;
    TA_Font font;
    std::string itemName;
    TA_Point velocity;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, TA_Point velocity, int itemNumber, std::string itemName);
    bool update() override;
    int getDrawPriority() override;
};

class TA_ItemLabel : public TA_Object {
private:
    const float showTime = 100;
    const float appearTime = 10;
    const float shift = 32;

    std::string name;
    TA_Font font;
    TA_Point currentPosition;
    float timer = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, std::string name);
    bool update() override;
    void draw() override;
    int getDrawPriority() override { return 2; }
};

#endif // TA_ITEM_BOX_H
