#ifndef TA_DATA_SELECT_SCREEN_H
#define TA_DATA_SELECT_SCREEN_H

#include "screen.h"

class TA_DataSelectScreen : public TA_Screen {
public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};

#endif // TA_DATA_SELECT_SCREEN_H
