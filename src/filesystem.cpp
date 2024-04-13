#include <SDL2/SDL.h>
#include "filesystem.h"
#include "error.h"

void TA::filesystem::fixPath(std::string &path)
{
    // SDL Android assets access breaks when adding ./ for some reason, so I'll just remove them
    while((int)path.size() >= 2 && path.substr(0, 2) == "./") {
        path.erase(0, 2);
    }
}

bool TA::filesystem::fileExists(std::string path)
{
    fixPath(path);
    SDL_RWops *file = SDL_RWFromFile(path.c_str(), "rb");
    if(file == nullptr) {
        return false;
    }

    if(SDL_RWclose(file) != 0) {
        TA::handleSDLError("Close %s after checking existence failed", path.c_str());
    }
    return true;
}

std::string TA::filesystem::readFile(std::string path)
{
    fixPath(path);
    SDL_RWops *file = SDL_RWFromFile(path.c_str(), "rb");
    if(file == nullptr) {
        TA::handleSDLError("Open %s for read failed", path.c_str());
    }

    int dataBytes = SDL_RWseek(file, 0, SEEK_END);
    SDL_RWseek(file, 0, SEEK_SET);
    char* data = new char[dataBytes];
    SDL_RWread(file, data, 1, dataBytes);

    std::string str(dataBytes + 1, 0);
    for(int pos = 0; pos < dataBytes; pos ++) {
        str[pos] = data[pos];
    }
    str += '\0';
    delete[] data;

    if(SDL_RWclose(file) != 0) {
        TA::handleSDLError("Close %s after reading failed", path.c_str());
    }
    return str;
}

std::string TA::filesystem::readAsset(std::string path)
{
    std::string prefix = getAssetsPath();
    return readFile(prefix + "/" + path);
}

const std::string TA::filesystem::getAssetsPath()
{
    #ifdef __ANDROID__
        return ".";
    #else
        #ifdef TA_LINUX_INSTALL
            return "/usr/share/tails-adventure";
        #else
            return "assets";
        #endif
    #endif
}

void TA::filesystem::writeFile(std::string path, std::string value)
{
    fixPath(path);
    SDL_RWops *file = SDL_RWFromFile(path.c_str(), "wb");
    if(file == nullptr) {
        TA::handleSDLError("Open %s for write failed", path.c_str());
    }

    for(int pos = 0; pos < (int)value.size(); pos ++) {
        SDL_RWwrite(file, &value[pos], sizeof(char), 1);
    }

    if(SDL_RWclose(file) != 0) {
        TA::handleSDLError("Close %s after writing failed", path.c_str());
    }
}
