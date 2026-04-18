#ifndef TA_CREDITS_SCREEN_H
#define TA_CREDITS_SCREEN_H

#include "font.h"
#include "screen.h"
#include "sound.h"
#include "sprite.h"

class TA_CreditsScreen : public TA_Screen {
public:
    void init() override;
    TA_ScreenState update() override;

private:
    void loadCredits();

    struct CreditsElement {
        std::vector<std::string> lines;
        bool head = false;
        bool sides = false;
    };

    TA_Sprite background;
    TA_Sprite tails;
    TA_Sprite bird;
    TA_Sprite robot;
    TA_Font font;

    std::vector<uint8_t> tailsFrame;
    std::vector<uint8_t> tailsX;
    std::vector<uint8_t> tailsY;
    std::vector<uint8_t> tailsFlip;
    std::vector<uint8_t> birdFrame;
    std::vector<uint8_t> birdX;
    std::vector<uint8_t> birdY;
    std::vector<uint8_t> birdFlip;
    std::vector<uint8_t> robotFrame;
    std::vector<CreditsElement> credits;

    TA_Sound smallExplosionSound;
    TA_Sound explosionSound;

    float timer = 0;

    std::array<TA_Sprite, 8> explosions;
    int explosionMinX = 0;
    int explosionMaxX = 0;
    int explosionMinY = 0;
    int explosionMaxY = 0;
    float explosionDelay = 6;
    float explosionTimer = 0;
    int explosionCount = 0;
    int prevRobotFrame = 0;
};

#endif
