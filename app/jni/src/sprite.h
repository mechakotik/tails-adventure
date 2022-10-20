#ifndef TA_SPRITE_H
#define TA_SPRITE_H

#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "geometry.h"
#include "camera.h"

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
    TA_Point position, scale{1, 1};
    TA_Camera *camera = nullptr;

    int animationFrame = 0, animationDelay = 1, repeatTimesLeft = -1;
    double animationTimer = 0;
    std::vector<int> animation;
    bool hidden = false;

public:
    void load(std::string filename, int frameWidth = -1, int frameHeight = -1);
    void loadFromTexture(TA_Texture *newTexture, int frameWidth = -1, int frameHeight = -1);
    void draw();
    void setPosition(TA_Point newPosition) {position = newPosition;}
    void setPosition(double newX, double newY) {setPosition(TA_Point(newX, newY));}
    void setAnimation(std::vector<int> newAnimation, int newAnimationDelay, int repeatTimes);
    void setAnimation(int firstFrame, int lastFrame, int newAnimationDelay, int repeatTimes);
    void setFrame(int newFrame);
    bool isAnimated();
    void setAlpha(int alpha);
    void setCamera(TA_Camera *newCamera) {camera = newCamera;}
    void setScale(TA_Point newScale) {scale = newScale;}
    void setColorMod(int r, int g, int b);
    void setColorMod(int w) {setColorMod(w, w, w);}
    ~TA_Sprite();
};

#endif //TA_SPRITE_H
