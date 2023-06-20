#ifndef TA_SPRITE_H
#define TA_SPRITE_H

#include <vector>
#include <string>
#include <map>
#include "SDL.h"
#include "SDL_image.h"
#include "geometry.h"
#include "camera.h"

class TA_Texture {
public:
    virtual void load(std::string filename);

    SDL_Texture *SDLTexture;
    int width, height;
};

class TA_Animation {
public:
    std::vector<int> frames;
    int delay = 1, repeatTimes = -1;

    void create(std::vector<int> newFrames, int newDelay, int newRepeatTimes);
    TA_Animation() {};
    TA_Animation(std::vector<int> frames, int delay, int repeatTimes);
    TA_Animation(int firstFrame, int lastFrame, int delay, int repeatTimes);
    TA_Animation(int frame);
};

class TA_Sprite {
private:
    TA_Texture texture;
    int frameWidth, frameHeight;
    int frame = 0;
    TA_Point position, scale{1, 1};
    TA_Camera *camera = nullptr;

    std::map<std::string, TA_Animation> loadedAnimations;
    TA_Animation animation;
    int animationFrame = 0;
    double animationTimer = 0;
    bool flip = false, hidden = false, updateAnimationNeeded = true, loaded = false;
    bool noPixelAspectRatio = false;
    int alpha = 255;
    std::string animationName;

    void updateAnimation();

public:
    void load(std::string filename, int frameWidth = -1, int frameHeight = -1);

    virtual void draw();
    void drawFrom(SDL_Rect srcRect);

    void setPosition(TA_Point newPosition) {position = newPosition;}
    void setPosition(double newX, double newY) {setPosition(TA_Point(newX, newY));}
    void setAlpha(int newAlpha);
    void setCamera(TA_Camera *newCamera) {camera = newCamera;}
    void setScale(TA_Point newScale) {scale = newScale;}
    void setColorMod(int r, int g, int b);
    void setColorMod(int w) {setColorMod(w, w, w);}
    void setFlip(bool newFlip) {flip = newFlip;}
    int getWidth() {return frameWidth;}
    int getHeight() {return frameHeight;}
    bool getFlip() {return flip;}
    TA_Point getPosition() {return position;}

    void loadAnimationsFromFile(std::string filename);
    void setAnimation(std::string name);
    void setAnimation(TA_Animation newAnimation);
    void setFrame(int newFrame);
    bool isAnimated();
    int getAnimationFrame();
    int getCurrentFrame();
    void disablePixelAspectRatio() {noPixelAspectRatio = true;}
    std::string getAnimationName() {return (isAnimated() ? animationName : "");}
};

#endif //TA_SPRITE_H
