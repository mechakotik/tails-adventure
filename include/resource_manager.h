#ifndef TA_RESOURCE_MANAGER_H
#define TA_RESOURCE_MANAGER_H

#include <string>
#include <SDL.h>
#include <SDL_mixer.h>

namespace TA { namespace resmgr {
    SDL_Texture* loadTexture(std::string filename);
    Mix_Chunk* loadChunk(std::string filename);
    void quit();
}}

#endif // TA_RESOURCE_MANAGER_H
