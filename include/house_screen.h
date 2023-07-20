#ifndef TA_HOUSE_SCREEN_H
#define TA_HOUSE_SCREEN_H

#include "screen_state_machine.h"
#include "sprite.h"
#include "font.h"
#include "controller.h"
#include "sound.h"
#include "inventory_menu.h"

class TA_HouseScreen : public TA_Screen {
private:
    const double curtainMoveTime = 10;
    const double curtainIdleTime = 10;

    void updatePositions();
    void updateSelector();
    void updateCurtain();
    bool shouldDoTransition();
    void applyTransition();

    void draw();
    void drawSelector();
    void drawCurtain();
    void drawCurtain(double factor);

    TA_InventoryMenu inventoryMenu;
    TA_Sprite interfaceSprite, houseSprite, curtainSprite;
    TA_Font font;
    TA_CommonController controller;
    TA_Sound switchSound, errorSound;

    int selectionX = 1, selectionY = 1;
    double curtainTimeLeft = -1;
    bool inventoryOpen = false, shouldMove = false, shouldExit = false;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override {}
};

#endif // TA_HOUSE_SCREEN_H
