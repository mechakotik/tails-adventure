#include "inventory_menu.h"
#include "controller.h"
#include "resource_manager.h"
#include "save.h"
#include "sound.h"

void TA_InventoryMenu::load(TA_Controller* controller) {
    this->controller = controller;

    itemSprite.load("hud/items.png", 16, 16);
    inventoryItemSprite.load("hud/items.png", 16, 16);
    pointerSprite.load("house/pointer.png");
    inventoryPointerSprite.load("house/pointer.png");
    arrowSprite.load("house/arrow.png", 8, 13);

    font.loadFont("fonts/item.toml");

    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
    selectSound.load("sound/select_item.ogg", TA_SOUND_CHANNEL_SFX2);
    backSound.load("sound/select.ogg", TA_SOUND_CHANNEL_SFX2);
    errorSound.load("sound/damage.ogg", TA_SOUND_CHANNEL_SFX2);

    loadOnscreenButtons();
}

void TA_InventoryMenu::loadOnscreenButtons() {
    for(int x = 0; x < 4; x++) {
        for(int y = 0; y < 2; y++) {
            buttons[x][y].setRectangle({0, 0}, {32, 24});
        }
        buttons[x][2].setRectangle({0, 0}, {22, 24});
    }

    leftButton.setRectangle({0, 0}, {32, 32});
    rightButton.setRectangle({0, 0}, {32, 32});
}

void TA_InventoryMenu::fillItemMatrix() {
    if(TA::save::getSaveParameter("seafox") == 1) {
        items[0][0] = {.number = 4, .name = getItemString("vulcan_gun")};
        items[0][1] = {.number = 22, .name = getItemString("anti_air_missile")};
        items[1][0] = {.number = 12, .name = getItemString("proton_torpedo")};
        items[1][1] = {.number = 23, .name = getItemString("spark")};
        items[2][0] = {.number = 20, .name = getItemString("extra_speed")};
        items[2][1] = {.number = 24, .name = getItemString("mine")};
        items[3][0] = {.number = 21, .name = getItemString("extra_armor")};
        items[3][1] = {.number = 25, .name = getItemString("rocket_booster")};
    }

    else {
        items[0][0] = {.number = 0, .name = getItemString("regular_bomb")};
        items[0][1] = {.number = 14, .name = getItemString("triple_bomb")};
        items[1][0] = {.number = 1, .name = getItemString("large_bomb")};
        items[1][1] = {.number = 15, .name = getItemString("wrench")};
        items[2][0] = {.number = 2, .name = getItemString("remote_bomb")};
        items[2][1] = {.number = 16, .name = getItemString("helmet")};
        items[3][0] = {.number = 13, .name = getItemString("napalm_bomb")};
        items[3][1] = {.number = 6, .name = getItemString("remote_robot")};
        items[4][0] = {.number = 19, .name = getItemString("hammer")};
        items[4][1] = {.number = 8, .name = getItemString("super_glove")};
        items[5][0] = {.number = 3, .name = getItemString("teleport_device")};
        items[5][1] = {.number = 9, .name = getItemString("fang")};
        items[6][0] = {.number = 5, .name = getItemString("night_vision")};
        items[6][1] = {.number = 10, .name = getItemString("knuckles")};
        items[7][0] = {.number = 7, .name = getItemString("speed_boots")};
        items[7][1] = {.number = 11, .name = getItemString("sonic")};
        items[8][0] = {.number = 17, .name = getItemString("item_radar")};
        items[8][1] = {.number = 33, .name = getItemString("purple_emerald")};
        items[9][0] = {.number = 18, .name = getItemString("radio")};
        items[9][1] = {.number = 31, .name = getItemString("red_emerald")};
        items[10][0] = {.number = 32, .name = getItemString("blue_emerald")};
        items[10][1] = {.number = 30, .name = getItemString("white_emerald")};
        items[11][0] = {.number = 29, .name = getItemString("green_emerald")};
        items[11][1] = {.number = 34, .name = getItemString("yellow_emerald")};
    }

    for(int x = 0; x < 12; x++) {
        for(int y = 0; y < 2; y++) {
            itemName[items[x][y].number] = items[x][y].name;
        }
    }

    replaceText = TA::resmgr::loadToml("hud/pause_menu_strings.toml").at("replace").as_string();
}

std::string TA_InventoryMenu::getItemString(const std::string& name) {
    return TA::resmgr::loadToml("house/item_strings.toml").at(name).as_string();
}

bool TA_InventoryMenu::update() {
    if(showTimeLeft <= 0 && hideTimeLeft <= 0 && listTransitionTimeLeft <= 0) {
        if(controller->isTouchscreen()) {
            selectingSlot = false;
        }
        updateOnscreenButtons();
        if(selectingSlot) {
            return updateSlotSelection();
        }
        return updateItemSelection();
    }
    return true;
}

void TA_InventoryMenu::updateOnscreenButtons() {
    for(int x = 0; x < 4; x++) {
        for(int y = 0; y < 2; y++) {
            float screenX = getLeftX() + 16 + 32 * x;
            float screenY = getTopY() + (y == 0 ? 36 : 60);
            if(replace) {
                screenY -= 2;
            }
            buttons[x][y].setPosition({screenX, screenY});
            buttons[x][y].update();
        }
        if(!replace) {
            buttons[x][2].setPosition(TA_Point(getLeftX() + 36 + 22 * x, getTopY() + 97));
            buttons[x][2].update();
        }
    }

    if(replace) {
        leftButton.setPosition(TA_Point(getLeftX() + 2, getTopY() + 79));
        rightButton.setPosition(TA_Point(getLeftX() + 126, getTopY() + 79));
    } else {
        leftButton.setPosition(TA_Point(getLeftX() - 16, getTopY() + 47));
        rightButton.setPosition(TA_Point(getLeftX() + 144, getTopY() + 47));
    }
    leftButton.update();
    rightButton.update();

    if(!replace) {
        for(int pos = 0; pos < 4; pos++) {
            if(buttons[pos][2].isReleased() && selectionSlot != pos) {
                selectSound.play();
                selectionSlot = pos;
                selectingSlot = false;
            }
        }
    }

    int page = selectionX / 4;
    if(leftButton.isReleased() && page - 1 >= 0) {
        prevSelection = {selectionX, selectionY};
        selectionX = (page - 1) * 4;
        selectionY = 0;
        listTransitionTimeLeft = listTransitionTime * 2;
        switchSound.play();
    } else if(rightButton.isReleased() && page + 1 < 3) {
        prevSelection = {selectionX, selectionY};
        selectionX = (page + 1) * 4;
        selectionY = 0;
        listTransitionTimeLeft = listTransitionTime * 2;
        switchSound.play();
    }
}

bool TA_InventoryMenu::updateSlotSelection() {
    if(controller->isJustChangedDirection()) {
        TA_Direction direction = controller->getDirection();
        int prev = selectionSlot;

        if(direction == TA_DIRECTION_LEFT) {
            selectionSlot = std::max(0, selectionSlot - 1);
        } else if(direction == TA_DIRECTION_RIGHT) {
            selectionSlot = std::min(3, selectionSlot + 1);
        }

        if(selectionSlot != prev) {
            switchSound.play();
        }
    }

    if(controller->isJustPressed(TA_BUTTON_A)) {
        selectSound.play();
        selectingSlot = false;
    }
    if(controller->isJustPressed(TA_BUTTON_B)) {
        backSound.play();
        return false;
    }
    return true;
}

bool TA_InventoryMenu::updateItemSelection() {
    if(!controller->isTouchscreen() && controller->isJustChangedDirection()) {
        TA_Direction direction = controller->getDirection();
        prevSelection = {selectionX, selectionY};

        switch(direction) {
            case TA_DIRECTION_UP:
                selectionY = std::max(0, selectionY - 1);
                break;
            case TA_DIRECTION_DOWN:
                selectionY = std::min(1, selectionY + 1);
                break;
            case TA_DIRECTION_LEFT:
                selectionX = std::max(0, selectionX - 1);
                break;
            case TA_DIRECTION_RIGHT:
                selectionX = std::min((TA::save::getSaveParameter("seafox") ? 3 : 11), selectionX + 1);
                break;
            default:
                break;
        }

        if(prevSelection != std::make_pair(selectionX, selectionY)) {
            switchSound.play();
            if(prevSelection.first / 4 != selectionX / 4) {
                listTransitionTimeLeft = listTransitionTime * 2;
            }
        }
    }

    bool shouldPlaceItem = controller->isJustPressed(TA_BUTTON_A);
    for(int x = 0; x < 4; x++) {
        for(int y = 0; y < 2; y++) {
            int page = selectionX / 4;
            if(buttons[x][y].isReleased()) {
                selectionX = page * 4 + x;
                selectionY = y;
                shouldPlaceItem = true;
            }
        }
    }

    if(shouldPlaceItem) {
        if(canPlaceItem(items[selectionX][selectionY].number)) {
            placeItem(selectionSlot, items[selectionX][selectionY].number);
            selectSound.play();
        } else {
            errorSound.play();
        }
        if(replace) {
            return false;
        }
        if(!controller->isTouchscreen()) {
            selectingSlot = true;
        }
    }

    if(controller->isJustPressed(TA_BUTTON_B)) {
        if(replace) {
            backSound.play();
            return false;
        }
        selectingSlot = true;
        backSound.play();
    }

    return true;
}

bool TA_InventoryMenu::canPlaceItem(int item) {
    if(!characterHasItem(item)) {
        return false;
    }
    if(item >= 29) {
        return false;
    }
    return true;
}

void TA_InventoryMenu::placeItem(int slot, int item) {
    int prevSlot = -1;
    for(int pos = 0; pos < 4; pos++) {
        if(getInventoryItem(pos) == item) {
            prevSlot = pos;
            break;
        }
    }

    if(prevSlot == -1) {
        setInventoryItem(slot, item);
    } else {
        int prevItem = getInventoryItem(slot);
        setInventoryItem(slot, item);
        setInventoryItem(prevSlot, prevItem);
    }
}

void TA_InventoryMenu::draw() {
    updateAlpha();
    drawItemList();
    if(!replace) {
        drawInventory();
        drawSelectionName();
    }
    drawPointer();
    drawArrows();
    if(replace) {
        drawSlotNumber();
    }
}

void TA_InventoryMenu::updateAlpha() {
    if(showTimeLeft > 0) {
        showTimeLeft -= TA::elapsedTime;
    }
    if(hideTimeLeft > 0) {
        hideTimeLeft -= TA::elapsedTime;
    }

    int globalAlpha = 0;
    if(showTimeLeft > 0) {
        globalAlpha = 255 - 255 * showTimeLeft / transitionTime;
    } else if(hideTimeLeft > 0) {
        globalAlpha = 255 * hideTimeLeft / transitionTime;
    } else if(shown) {
        globalAlpha = 255;
    }

    inventoryItemSprite.setAlpha(globalAlpha);
    font.setAlpha(globalAlpha);
    inventoryPointerSprite.setAlpha(globalAlpha);

    arrowTimer += TA::elapsedTime;
    arrowTimer = fmod(arrowTimer, (arrowIdleTime + arrowTransitionTime) * 2);
    int arrowAlpha = 0;

    if(arrowTimer < arrowTransitionTime) {
        arrowAlpha = 255 * arrowTimer / arrowTransitionTime;
    } else if(arrowTimer < arrowTransitionTime + arrowIdleTime) {
        arrowAlpha = 255;
    } else if(arrowTimer < arrowTransitionTime * 2 + arrowIdleTime) {
        arrowAlpha = 255 - 255 * (arrowTimer - arrowIdleTime - arrowTransitionTime) / arrowTransitionTime;
    } else {
        arrowAlpha = 0;
    }

    arrowAlpha = std::min(arrowAlpha, globalAlpha);
    arrowSprite.setAlpha(arrowAlpha);

    int itemAlpha = 255;

    if(listTransitionTimeLeft > 0) {
        listTransitionTimeLeft -= TA::elapsedTime;
        if(listTransitionTimeLeft > listTransitionTime) {
            itemAlpha = 255 * (listTransitionTimeLeft - listTransitionTime) / listTransitionTime;
        } else {
            itemAlpha = 255 - 255 * listTransitionTimeLeft / listTransitionTime;
        }
    }

    itemAlpha = std::min(itemAlpha, globalAlpha);
    itemSprite.setAlpha(itemAlpha);
    pointerSprite.setAlpha(itemAlpha);
}

void TA_InventoryMenu::drawItemList() {
    int page = selectionX / 4;
    if(listTransitionTimeLeft > listTransitionTime) {
        page = prevSelection.first / 4;
    }

    for(int x = page * 4; x < (page + 1) * 4; x++) {
        for(int y = 0; y < 2; y++) {
            float screenX = getLeftX() + 24 + 32 * (x % 4);
            float screenY = getTopY() + (y == 0 ? 40 : 64);
            if(replace) {
                screenY -= 2;
            }
            itemSprite.setPosition(screenX, screenY);

            int index = items[x][y].number;
            if(characterHasItem(index)) {
                itemSprite.setFrame(index);
            } else {
                itemSprite.setFrame(38);
            }

            itemSprite.draw();
        }
    }
}

void TA_InventoryMenu::drawInventory() {
    inventoryItemSprite.setPosition(getLeftX() + 39, getTopY() + 101);

    for(int pos = 0; pos < 4; pos++) {
        int index = getInventoryItem(pos);
        if(index == -1) {
            index = 38;
        }
        inventoryItemSprite.setFrame(index);
        inventoryItemSprite.draw();
        inventoryItemSprite.setPosition(inventoryItemSprite.getPosition() + TA_Point(22, 0));
    }
}

int TA_InventoryMenu::getInventoryItem(int slot) {
    std::string paramName = getParameterName(slot);
    return TA::save::getSaveParameter(paramName);
}

void TA_InventoryMenu::setInventoryItem(int slot, int index) {
    std::string paramName = getParameterName(slot);
    TA::save::setSaveParameter(paramName, index);
}

std::string TA_InventoryMenu::getParameterName(int slot) {
    bool seaFox = TA::save::getSaveParameter("seafox");
    return (seaFox ? "seafox_item_slot" : "item_slot") + std::to_string(slot);
}

void TA_InventoryMenu::drawSelectionName() {
    TA_Point position = TA_Point(getLeftX() + 16, getTopY() + 128);
    std::string text = "";

    if(selectingSlot) {
        int item = getInventoryItem(selectionSlot);
        if(itemName.count(item)) {
            text = itemName[item];
        }
    } else {
        if(characterHasItem(items[selectionX][selectionY].number)) {
            text = items[selectionX][selectionY].name;
        } else {
            text = "???";
        }
    }

    font.drawText(position, text);
}

bool TA_InventoryMenu::characterHasItem(int item) {
    long long itemMask = TA::save::getSaveParameter("item_mask");
    return itemMask & (1ll << item);
}

void TA_InventoryMenu::drawPointer() {
    if(controller->isTouchscreen()) {
        drawPointerTouchscreen();
    } else {
        drawPointerController();
    }
}

void TA_InventoryMenu::drawPointerController() {
    if(!replace) {
        TA_Point inventoryPosition = TA_Point(getLeftX() + 39 + 22 * selectionSlot, getTopY() + 101);
        inventoryPointerSprite.setPosition(inventoryPosition - TA_Point(1, 2));
        inventoryPointerSprite.draw();
        if(selectingSlot) {
            return;
        }
    }

    int pointerX = selectionX, pointerY = selectionY;
    if(listTransitionTimeLeft > listTransitionTime) {
        std::tie(pointerX, pointerY) = prevSelection;
    }

    float screenX = getLeftX() + 24 + 32 * (pointerX % 4);
    float screenY = getTopY() + (pointerY == 0 ? 40 : 64);
    if(replace) {
        screenY -= 2;
    }
    pointerSprite.setPosition(screenX - 1, screenY - 2);
    pointerSprite.draw();
}

void TA_InventoryMenu::drawPointerTouchscreen() {
    if(!replace) {
        for(int pos = 0; pos < 4; pos++) {
            if((selectingSlot && buttons[pos][2].isPressed()) || (!selectingSlot && pos == selectionSlot)) {
                TA_Point position = TA_Point(getLeftX() + 39 + 22 * pos, getTopY() + 101);
                inventoryPointerSprite.setPosition(position - TA_Point(1, 2));
                inventoryPointerSprite.draw();
            }
        }
    }
    if(!selectingSlot) {
        for(int x = 0; x < 4; x++) {
            for(int y = 0; y < 2; y++) {
                if(buttons[x][y].isPressed()) {
                    float screenX = getLeftX() + 24 + 32 * x;
                    float screenY = getTopY() + (y == 0 ? 40 : 64);
                    if(replace) {
                        screenY -= 2;
                    }
                    pointerSprite.setPosition(screenX - 1, screenY - 2);
                    pointerSprite.draw();
                }
            }
        }
    }
}

void TA_InventoryMenu::drawArrows() {
    if(TA::save::getSaveParameter("seafox")) {
        return;
    }
    if(selectionX >= 4) {
        if(replace) {
            arrowSprite.setPosition(getLeftX() + 26, getTopY() + 89);
        } else {
            arrowSprite.setPosition(getLeftX() + 8, getTopY() + 57);
        }
        arrowSprite.setFrame(0);
        arrowSprite.draw();
    }
    if(selectionX <= 7) {
        if(replace) {
            arrowSprite.setPosition(getLeftX() + 126, getTopY() + 89);
        } else {
            arrowSprite.setPosition(getLeftX() + 144, getTopY() + 57);
        }
        arrowSprite.setFrame(1);
        arrowSprite.draw();
    }
}

void TA_InventoryMenu::drawSlotNumber() {
    std::string text = replaceText + " " + std::to_string(selectionSlot + 1);
    font.drawTextCentered(getTopY() + 91, text);
}

void TA_InventoryMenu::show() {
    if(shown) {
        return;
    }

    fillItemMatrix();
    showTimeLeft = transitionTime;
    hideTimeLeft = -1;
    shown = true;
    selectionSlot = selectionX = selectionY = 0;
    if(replace) {
        std::string param = (TA::save::getSaveParameter("seafox") == 1 ? "seafox_item_position" : "item_position");
        selectionSlot = TA::save::getSaveParameter(param);
        selectingSlot = false;
    }
}

void TA_InventoryMenu::hide() {
    if(!shown) {
        return;
    }
    hideTimeLeft = transitionTime;
    showTimeLeft = -1;
    shown = false;
}
