#ifndef TA_PAUSE_MENU_H
#define TA_PAUSE_MENU_H

#include <array>
#include "sprite.h"
#include "touchscreen.h"
#include "font.h"
#include "links.h"
#include "sound.h"

class TA_PauseMenu {
public:
    enum class UpdateResult : char {
        CONTINUE,
        RESUME,
        QUIT
    };

    void load(const TA_Links& links);
    UpdateResult update();
    void setAlpha(const int& alpha);
    void draw();
    void reset();

private:
    void processTouchInput();
    void processControllerInput();
    void select();
    void drawHighlight(const double& y) const;

    TA_Sprite itemSprite, pointerSprite, frameSprite;
    TA_Links links;
    std::array<TA_OnscreenButton, 4> itemButtons;
    std::array<TA_OnscreenButton, 3> menuButtons;
    TA_Font font;
    TA_Sound switchSound, pauseSound;

    UpdateResult result = UpdateResult::CONTINUE;
    int selection = 0, itemPosition = 0;
    int globalAlpha = 0;
};

#endif // TA_PAUSE_MENU_H
