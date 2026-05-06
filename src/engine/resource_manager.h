#ifndef TA_RESOURCE_MANAGER_H
#define TA_RESOURCE_MANAGER_H

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <filesystem>
#include <toml.hpp>

namespace TA {
    namespace resmgr {
        void load();
        SDL_Texture* loadTexture(std::filesystem::path path);
        MIX_Audio* loadMusic(std::filesystem::path path);
        MIX_Audio* loadChunk(std::filesystem::path path);
        const std::string& loadAsset(std::filesystem::path path);
        const toml::value& loadToml(std::filesystem::path path);
        int getTotalMods();
        int getLoadedMods();
        void quit();
    }
}

#endif // TA_RESOURCE_MANAGER_H
