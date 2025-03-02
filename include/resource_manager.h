#ifndef TA_RESOURCE_MANAGER_H
#define TA_RESOURCE_MANAGER_H

#include <string>
#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"

namespace TA {
    namespace resmgr {
        void preload();
        SDL_Texture* loadTexture(std::string filename);
        Mix_Music* loadMusic(std::string filename);
        Mix_Chunk* loadChunk(std::string filename);
        std::string loadAsset(std::string filename);
        void quit();
    }
}

#endif // TA_RESOURCE_MANAGER_H
