#ifndef TA_TEXTURE_H
#define TA_TEXTURE_H

#include <SDL.h>
#include <SDL_image.h>

class TA_Texture {
protected:
    ~TA_Texture();
    virtual void load(const char *filename);

    SDL_Texture *texture;
    int textureWidth, textureHeight;
};

#endif //TA_TEXTURE_H
