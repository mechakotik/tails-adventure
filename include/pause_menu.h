#ifndef TA_PAUSE_MENU_H
#define TA_PAUSE_MENU_H

#include <array>
#include "inventory_menu.h"
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
        REPLACE,
        QUIT
    };

    void load(TA_Links links);
    UpdateResult update();
    void setAlpha(int alpha);
    void draw();
    void reset();

private:
    class SwitchMenu {
    public:
        void load(TA_Links links);
        UpdateResult update();
        void draw();
        void setAlpha(int alpha);
        void reset();

    private:
        void processTouchInput();
        void processControllerInput();
        void select();
        void drawHighlight(double y) const;

        TA_InventoryMenu inventoryMenu;
        TA_Sprite itemSprite, pointerSprite;
        TA_Links links;
        std::array<TA_OnscreenButton, 4> itemButtons;
        std::array<TA_OnscreenButton, 3> menuButtons;
        TA_Font font;
        TA_Sound switchSound, selectSound, pauseSound;

        UpdateResult result = UpdateResult::CONTINUE;
        int selection = 0, itemPosition = 0;
        int globalAlpha = 0;
    };

    static constexpr double transitionTime = 6;

    TA_Sprite frameSprite;
    SwitchMenu switchMenu;
    TA_InventoryMenu inventoryMenu;
    int globalAlpha = 0;

    bool replace = false;
    bool replaceWanted = false;
    double timer = 0;
};

#endif // TA_PAUSE_MENU_H
