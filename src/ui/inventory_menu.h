#ifndef TA_INVENTORY_MENU_H
#define TA_INVENTORY_MENU_H

#include "controller.h"
#include "font.h"
#include "sound.h"
#include "sprite.h"

class TA_InventoryMenu {
private:
    const float transitionTime = 6;
    const float arrowIdleTime = 15;
    const float arrowTransitionTime = 5;
    const float listTransitionTime = 6;

    void fillItemMatrix();
    void loadOnscreenButtons();
    static std::string getItemString(const std::string& name);

    bool updateSlotSelection();
    bool updateItemSelection();
    void updateOnscreenButtons();
    void updateAlpha();

    void drawItemList();
    void drawInventory();
    void drawSelectionName();
    void drawPointer();
    void drawPointerController();
    void drawPointerTouchscreen();
    void drawArrows();
    void drawSlotNumber();

    int getInventoryItem(int slot);
    void setInventoryItem(int slot, int value);
    std::string getParameterName(int slot);
    int getLeftX() { return TA::screenWidth / 2 - 80; }
    int getTopY() { return (TA::screenHeight - 144) / 2; }
    bool characterHasItem(int item);
    bool canPlaceItem(int item);
    void placeItem(int slot, int item);

    struct Item {
        int number;
        std::string name;
    };

    std::array<std::array<Item, 2>, 12> items;
    std::map<int, std::string> itemName;
    std::string replaceText;

    TA_Controller* controller;
    TA_Sprite itemSprite, pointerSprite, arrowSprite;
    TA_Sprite inventoryItemSprite, inventoryPointerSprite;
    TA_Sound switchSound, selectSound, backSound, errorSound;
    TA_Font font;

    std::array<std::array<TA_OnscreenButton, 3>, 4> buttons;
    TA_OnscreenButton leftButton, rightButton;

    int selectionX = 0, selectionY = 0, selectionSlot = 0;
    std::pair<int, int> prevSelection;
    bool selectingSlot = true, shown = false;
    bool replace = false;
    float arrowTimer = 0, showTimeLeft = -1, hideTimeLeft = -1, listTransitionTimeLeft = -1;

public:
    TA_InventoryMenu() {}
    void load(TA_Controller* controller);
    bool update();
    void draw();
    void show();
    void hide();
    bool isShown() { return shown || hideTimeLeft > 0; }
    void setReplace(bool replace) { this->replace = replace; }
};

#endif // TA_INVENTORY_MENU_H
