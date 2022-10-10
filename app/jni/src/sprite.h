#ifndef TA_SPRITE_H
#define TA_SPRITE_H

#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

class TA_Texture {
public:
    virtual void load(std::string filename);
    ~TA_Texture();

    SDL_Texture *SDLTexture;
    int width, height, spritesUsed = 0;
};

class TA_Sprite {
private:
    TA_Texture *texture;
    int frameWidth, frameHeight;
    int frame = 0;
    double xpos = 0, ypos = 0;

    int animationFrame = 0, animationDelay = 1, repeatTimesLeft = -1;
    double animationTimer = 0;
    std::vector<int> animation;

public:
    void load(std::string filename, int frameWidth = -1, int frameHeight = -1);
    void loadFromTexture(TA_Texture *newTexture, int frameWidth = -1, int frameHeight = -1);
    void draw();
    void setPosition(double newXpos, double newYpos);
    void setAnimation(std::vector<int> newAnimation, int newAnimationDelay, int repeatTimes);
    void setAnimation(int firstFrame, int lastFrame, int newAnimationDelay, int repeatTimes);
    void setFrame(int newFrame);
    bool isAnimated();
    void setAlpha(int alpha);
    ~TA_Sprite();
};

#endif //TA_SPRITE_H
