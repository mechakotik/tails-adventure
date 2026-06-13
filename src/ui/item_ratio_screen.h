#ifndef TA_ITEM_RATIO_SCREEN_H
#define TA_ITEM_RATIO_SCREEN_H

#include "font.h"
#include "screen.h"

class TA_ItemRatioScreen : public TA_Screen {
public:
    void init() override;
    TA_ScreenState update() override;

private:
    TA_Font font;
    int ratio = 0;
    float timer = 0;
};

#endif
