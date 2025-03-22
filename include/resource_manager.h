#ifndef TA_RESOURCE_MANAGER_H
#define TA_RESOURCE_MANAGER_H

#include <filesystem>
#include <toml.hpp>
#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"

namespace TA {
    namespace resmgr {
        void load();
        SDL_Texture* loadTexture(std::filesystem::path path);
        Mix_Music* loadMusic(std::filesystem::path path);
        Mix_Chunk* loadChunk(std::filesystem::path path);
        const std::string& loadAsset(std::filesystem::path path);
        const toml::value& loadToml(std::filesystem::path path);
        void quit();
    }
}

#endif // TA_RESOURCE_MANAGER_H
