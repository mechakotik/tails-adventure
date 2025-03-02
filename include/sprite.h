#ifndef TA_SPRITE_H
#define TA_SPRITE_H

#include <filesystem>
#include <map>
#include <string>
#include <vector>
#include "SDL3/SDL.h"
#include "camera.h"
#include "geometry.h"

class TA_Texture {
public:
    virtual void load(std::string filename);

    SDL_Texture* SDLTexture = nullptr;
    int width = 0, height = 0;
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
    int frameWidth = 0, frameHeight = 0;
    int frame = 0;
    TA_Point position;
    TA_Camera* camera = nullptr;

    std::map<std::string, TA_Animation> loadedAnimations;
    TA_Animation animation;
    int animationFrame = 0;
    double animationTimer = 0;
    bool flip = false, hidden = false, updateAnimationNeeded = true, loaded = false;
    bool doUpdateAnimation = true;
    int alpha = 255;
    std::string animationName;

    void tryLoadFromToml(std::filesystem::path path);

public:
    void load(std::string filename, int frameWidth = -1, int frameHeight = -1);
    void loadFromToml(std::filesystem::path path);

    virtual void draw();
    void drawFrom(SDL_Rect srcRect);

    void setPosition(TA_Point newPosition) { position = newPosition; }
    void setPosition(double newX, double newY) { setPosition(TA_Point(newX, newY)); }
    void setAlpha(int newAlpha);
    void setCamera(TA_Camera* newCamera) { camera = newCamera; }
    void setColorMod(int r, int g, int b);
    void setColorMod(int w) { setColorMod(w, w, w); }
    void setFlip(bool newFlip) { flip = newFlip; }
    int getWidth() { return frameWidth; }
    int getHeight() { return frameHeight; }
    bool getFlip() { return flip; }
    TA_Point getPosition() { return position; }

    void setAnimation(std::string name);
    void setAnimation(TA_Animation newAnimation);
    void setFrame(int newFrame);
    bool isAnimated();
    int getAnimationFrame();
    int getCurrentFrame();
    std::string getAnimationName() { return (isAnimated() ? animationName : ""); }
    void updateAnimation();
    void setUpdateAnimation(bool enabled) { doUpdateAnimation = enabled; }
};

#endif // TA_SPRITE_H
