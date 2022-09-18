#ifndef TA_SPRITE_H
#define TA_SPRITE_H

#include <vector>
#include <SDL.h>
#include "TA_Texture.h"

class TA_Sprite : TA_Texture {
private:
    int frameWidth, frameHeight;
    int frame = 0;
    double xpos = 0, ypos = 0;

    int animationFrame = 0, animationTimer = 0, animationDelay = 1, repeatTimesLeft = -1;
    std::vector<int> animation;

public:
    void load(const char *filename, int frameWidth, int frameHeight);
    void draw();
    void setPosition(double newXpos, double newYpos);
    void setAnimation(std::vector<int> newAnimation, int newAnimationDelay, int repeatTimes);
    void setAnimation(int firstFrame, int lastFrame, int newAnimationDelay, int repeatTimes);
    void setFrame(int newFrame);
    bool isAnimated();
};

#endif //TA_SPRITE_H
