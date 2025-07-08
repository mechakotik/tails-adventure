#ifndef TA_PAUSE_MENU_H
#define TA_PAUSE_MENU_H

#include <array>
#include "font.h"
#include "inventory_menu.h"
#include "links.h"
#include "sound.h"
#include "sprite.h"
#include "touchscreen.h"

class TA_PauseMenu {
public:
    enum class UpdateResult : char { CONTINUE, RESUME, REPLACE, QUIT };

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
        void drawHighlight(float y) const;

        TA_InventoryMenu inventoryMenu;
        TA_Sprite itemSprite, pointerSprite;
        TA_Font font;
        TA_Sound switchSound, selectSound, pauseSound;
        TA_Links links;

        std::array<std::string, 3> menu;
        std::array<TA_OnscreenButton, 4> itemButtons;
        std::array<TA_OnscreenButton, 3> menuButtons;

        UpdateResult result = UpdateResult::CONTINUE;
        int selection = 0, itemPosition = 0;
        int globalAlpha = 0;
    };

    static constexpr float transitionTime = 6;

    TA_Sprite frameSprite;
    SwitchMenu switchMenu;
    TA_InventoryMenu inventoryMenu;
    int globalAlpha = 0;

    bool replace = false;
    bool replaceWanted = false;
    float timer = 0;
};

#endif // TA_PAUSE_MENU_H
