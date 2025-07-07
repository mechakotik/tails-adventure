#include "object_set.h"

class TA_ElectricBarrier : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(int top, int left, int bottom, int right, TA_Point switchPosition);
    bool update() override;
    void draw() override;

private:
    int top, left, bottom, right;
    TA_Sprite switchSprite;
    TA_Rect switchHitbox;
    TA_Sound switchSound;
};
