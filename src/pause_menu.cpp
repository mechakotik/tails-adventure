#include "pause_menu.h"
#include <SDL3/SDL_render.h>
#include <array>
#include <cstddef>
#include <string>
#include "SDL3/SDL_rect.h"
#include "controller.h"
#include "links.h"
#include "resource_manager.h"
#include "save.h"
#include "sound.h"
#include "tools.h"

void TA_PauseMenu::load(TA_Links links) {
    switchMenu.load(links);
    inventoryMenu.load(links.controller);
    inventoryMenu.setReplace(true);

    frameSprite.load("hud/pause_menu_frame.png");
    frameSprite.setPosition(static_cast<float>(TA::screenWidth - frameSprite.getWidth()) / 2,
        static_cast<float>(TA::screenHeight - frameSprite.getHeight()) / 2);

    reset();
}

TA_PauseMenu::UpdateResult TA_PauseMenu::update() {
    if(replace != replaceWanted) {
        timer += TA::elapsedTime;
        if(timer > transitionTime) {
            replace = replaceWanted;
            if(replace) {
                inventoryMenu.show();
            }
        } else {
            if(replace) {
                switchMenu.setAlpha(255 * timer / transitionTime);
            } else {
                switchMenu.setAlpha(255 - 255 * timer / transitionTime);
            }
            return UpdateResult::CONTINUE;
        }
    }

    timer = 0;
    if(replace) {
        if(!inventoryMenu.update()) {
            inventoryMenu.hide();
        }
        if(!inventoryMenu.isShown()) {
            replaceWanted = false;
            switchMenu.setAlpha(0);
        }
        return UpdateResult::CONTINUE;
    }

    UpdateResult res = switchMenu.update();
    if(res == UpdateResult::REPLACE) {
        replaceWanted = true;
        return UpdateResult::CONTINUE;
    }
    return res;
}

void TA_PauseMenu::setAlpha(int alpha) {
    frameSprite.setAlpha(alpha);
    switchMenu.setAlpha(alpha);
    globalAlpha = alpha;
}

void TA_PauseMenu::reset() {
    setAlpha(0);
    switchMenu.reset();
}

void TA_PauseMenu::draw() {
    TA::drawScreenRect(0, 0, 0, globalAlpha / 2);
    frameSprite.draw();
    if(replace && replaceWanted) {
        inventoryMenu.draw();
    } else {
        switchMenu.draw();
    }
}

void TA_PauseMenu::SwitchMenu::load(TA_Links links) {
    itemSprite.load("hud/items.png", 16, 16);
    pointerSprite.load("house/pointer.png");
    font.loadFont("fonts/pause_menu.toml");

    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
    selectSound.load("sound/select_item.ogg", TA_SOUND_CHANNEL_SFX2);
    pauseSound.load("sound/enter.ogg", TA_SOUND_CHANNEL_SFX2);

    for(size_t pos = 0; pos < 4; pos += 1) {
        itemButtons.at(pos).setRectangle({0, 0}, {32, 20});
    }
    for(size_t pos = 0; pos < 3; pos += 1) {
        menuButtons.at(pos).setRectangle({0, 0}, {125, 17});
    }

    toml::value table = TA::resmgr::loadToml("hud/pause_menu_strings.toml");
    menu[0] = table.at("continue").as_string();
    menu[1] = table.at("replace_item").as_string();
    menu[2] = table.at("quit_to_map").as_string();

    this->links = links;
    reset();
}

TA_PauseMenu::UpdateResult TA_PauseMenu::SwitchMenu::update() {
    result = UpdateResult::CONTINUE;
    if(links.controller->isTouchscreen()) {
        processTouchInput();
    } else {
        processControllerInput();
    }
    return result;
}

void TA_PauseMenu::SwitchMenu::reset() {
    const std::string itemPositionKey = (links.seaFox == nullptr ? "item_position" : "seafox_item_position");
    itemPosition = static_cast<int>(TA::save::getSaveParameter(itemPositionKey));
    selection = 0;
}

void TA_PauseMenu::SwitchMenu::processControllerInput() {
    if(links.controller->isJustPressed(TA_BUTTON_PAUSE) || links.controller->isJustPressed(TA_BUTTON_A)) {
        select();
        return;
    }
    if(links.controller->isJustPressed(TA_BUTTON_B)) {
        result = UpdateResult::RESUME;
        pauseSound.play();
        return;
    }

    if(!links.controller->isJustChangedDirection()) {
        return;
    }

    const TA_Direction direction = links.controller->getDirection();
    const std::string itemPositionKey = (links.seaFox == nullptr ? "item_position" : "seafox_item_position");

    if(direction == TA_DIRECTION_LEFT && itemPosition >= 1) {
        switchSound.play();
        itemPosition -= 1;
        TA::save::setSaveParameter(itemPositionKey, itemPosition);
    } else if(direction == TA_DIRECTION_RIGHT && itemPosition <= 2) {
        switchSound.play();
        itemPosition += 1;
        TA::save::setSaveParameter(itemPositionKey, itemPosition);
    } else if(direction == TA_DIRECTION_UP && selection >= 1) {
        switchSound.play();
        selection -= 1;
    } else if(direction == TA_DIRECTION_DOWN && selection <= 1) {
        switchSound.play();
        selection += 1;
    }
}

void TA_PauseMenu::SwitchMenu::processTouchInput() {
    const float startX = (static_cast<float>(TA::screenWidth) / 2) - 64;
    const float startY = static_cast<float>(TA::screenHeight - 144) / 2;

    for(int pos = 0; pos < 3; pos++) {
        menuButtons.at(pos).setPosition(
            {(static_cast<float>(TA::screenWidth) / 2) - 62, startY + static_cast<float>(63 + (17 * pos))});
        menuButtons.at(pos).update();
        if(menuButtons.at(pos).isReleased()) {
            selection = pos;
            select();
            return;
        }
    }

    const std::string itemPositionKey = (links.seaFox == nullptr ? "item_position" : "seafox_item_position");
    for(int pos = 0; pos < 4; pos += 1) {
        itemButtons.at(pos).setPosition({startX + (pos * 32), startY + 36});
        itemButtons.at(pos).update();
        if(itemButtons.at(pos).isJustPressed()) {
            switchSound.play();
            TA::save::setSaveParameter(itemPositionKey, pos);
            itemPosition = pos;
        }
    }
}

void TA_PauseMenu::SwitchMenu::select() {
    switch(selection) {
        case 0:
            result = UpdateResult::RESUME;
            pauseSound.play();
            break;
        case 1:
            result = UpdateResult::REPLACE;
            selectSound.play();
            break;
        case 2:
            result = UpdateResult::QUIT;
            break;
        default:
            break;
    }
}

void TA_PauseMenu::SwitchMenu::setAlpha(int alpha) {
    itemSprite.setAlpha(alpha);
    pointerSprite.setAlpha(alpha);
    globalAlpha = alpha;
    font.setAlpha(alpha);
}

void TA_PauseMenu::SwitchMenu::draw() {
    const float startX = (static_cast<float>(TA::screenWidth) / 2) - 56;
    const float startY = static_cast<float>(TA::screenHeight - 144) / 2;

    for(int num = 0; num < 4; num++) {
        const std::string itemKey = (links.seaFox == nullptr ? "item_slot" : "seafox_item_slot") + std::to_string(num);
        int item = static_cast<int>(TA::save::getSaveParameter(itemKey));
        if(item == -1) {
            item = 38;
        }

        itemSprite.setPosition(startX + (num * 32), startY + 38);
        itemSprite.setFrame(item);
        itemSprite.draw();
    }

    pointerSprite.setPosition(startX + (itemPosition * 32) - 1, startY + 36);
    pointerSprite.draw();

    const bool touchscreen = links.controller->isTouchscreen();
    for(int pos = 0; pos < menu.size(); pos += 1) {
        if((!touchscreen && selection == pos) || (touchscreen && menuButtons.at(pos).isPressed())) {
            drawHighlight(startY + 60 + (17 * pos));
        }
        font.drawTextCentered(startY + 63 + (17 * pos), menu.at(pos), {-1, 0});
    }
}

void TA_PauseMenu::SwitchMenu::drawHighlight(float y) const {
    SDL_FRect rect = {(static_cast<float>(TA::screenWidth) / 2) - 54, static_cast<float>(y), 110, 15};

    for(int num = 0; num < 4; num++) {
        const int squareAlpha = globalAlpha * globalAlpha / 255;
        SDL_SetRenderDrawColor(TA::renderer, num * 28, num * 24, num * 28, squareAlpha);

        SDL_FRect targetRect = rect;
        targetRect.x *= static_cast<float>(TA::scaleFactor);
        targetRect.y *= static_cast<float>(TA::scaleFactor);
        targetRect.w *= static_cast<float>(TA::scaleFactor);
        targetRect.h *= static_cast<float>(TA::scaleFactor);

        SDL_RenderFillRect(TA::renderer, &targetRect);
        rect.x += 2;
        rect.w -= 4;
    }
}
