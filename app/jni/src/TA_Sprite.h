#ifndef TA_SPRITE_H
#define TA_SPRITE_H

#include <vector>
#include <SDL.h>
#include "TA_Texture.h"

class TA_Sprite : TA_Texture {
private:
    int frameWidth, frameHeight;
    int frame = 0, xpos = 0, ypos = 0;

    int animationFrame = 0, animationTimer = 0, animationDelay = 1;
    std::vector<int> animation;

public:
    void load(const char *filename, int frameWidth, int frameHeight);
    void draw();
    void setPosition(int newXpos, int newYpos);
    void setAnimation(std::vector<int> newAnimation, int newAnimationDelay);
};

#endif //TA_SPRITE_H
