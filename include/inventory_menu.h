#ifndef TA_INVENTORY_MENU_H
#define TA_INVENTORY_MENU_H

#include "sprite.h"
#include "controller.h"
#include "font.h"
#include "sound.h"

class TA_InventoryMenu {
private:
    const double transitionTime = 10;
    const double arrowIdleTime = 15;
    const double arrowTransitionTime = 5;
    const double listTransitionTime = 10;

    void fillItemMatrix();

    bool updateSlotSelection();
    void updateItemSelection();
    void updateAlpha();

    void drawItemList();
    void drawInventory();
    void drawSelectionName();
    void drawPointer();
    void drawArrows();

    int getInventoryItem(int slot);
    void setInventoryItem(int slot, int value);
    int getLeftX() {return TA::screenWidth / 2 - 80;}
    bool characterHasItem(int item);
    bool canPlaceItem(int item);
    void placeItem(int slot, int item);

    struct Item {
        int number;
        std::string name;
    };

    std::array<std::array<Item, 2>, 12> items;
    std::map<int, std::string> itemName;

    TA_CommonController* controller;
    TA_Sprite itemSprite, pointerSprite, arrowSprite;
    TA_Sprite inventoryItemSprite, inventoryPointerSprite;
    TA_Sound switchSound;
    TA_Font font;

    int selectionX = 0, selectionY = 0, selectionSlot = 0;
    std::pair<int, int> prevSelection;
    bool selectingSlot = true, shown = false;
    double arrowTimer = 0, showTimeLeft = -1, hideTimeLeft = -1, listTransitionTimeLeft = -1;

public:
    void load(TA_CommonController* controller);
    bool update();
    void draw();
    void show();
    void hide();
    bool isShown() {return shown || hideTimeLeft > 0;}
};

#endif // TA_INVENTORY_MENU_H
