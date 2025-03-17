#include "filesystem.h"
#include <filesystem>
#include "SDL3/SDL.h"
#include "error.h"

#ifdef _WIN32
#include "windows.h"
#elif __linux__
#include <climits>
#include <unistd.h>
#endif

bool TA::filesystem::fileExists(std::filesystem::path path) {
    SDL_IOStream* file = SDL_IOFromFile(path.c_str(), "rb");
    if(file == nullptr) {
        return false;
    }

    if(!SDL_CloseIO(file)) {
        TA::handleSDLError("close %s after checking existence failed", path.c_str());
    }
    return true;
}

std::string TA::filesystem::readFile(std::filesystem::path path) {
    SDL_IOStream* file = SDL_IOFromFile(path.c_str(), "rb");
    if(file == nullptr) {
        TA::handleSDLError("open %s for read failed", path.c_str());
    }

    size_t dataBytes = SDL_SeekIO(file, 0, SDL_IO_SEEK_END);
    SDL_SeekIO(file, 0, SDL_IO_SEEK_SET);
    char* data = new char[dataBytes];
    SDL_ReadIO(file, data, dataBytes);

    std::string str(dataBytes, 0);
    for(int pos = 0; pos < dataBytes; pos++) {
        str[pos] = data[pos];
    }
    delete[] data;

    if(!SDL_CloseIO(file)) {
        TA::handleSDLError("close %s after reading failed", path.c_str());
    }
    return str;
}

std::string TA::filesystem::readAsset(std::filesystem::path path) {
    return readFile(getAssetsPath() / path);
}

std::filesystem::path TA::filesystem::getAssetsPath() {
#ifdef __ANDROID__
    return "";
#elifdef TA_UNIX_INSTALL
    return "/usr/share/tails-adventure";
#else
    return getExecutableDirectory() / "assets";
#endif
}

std::filesystem::path TA::filesystem::getExecutableDirectory() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string path(buffer);
    return path.substr(0, path.find_last_of("\\/"));
#else
    char buffer[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
    std::string path(buffer, (count > 0 ? count : 0));
    return path.substr(0, path.find_last_of("/"));
#endif
}

void TA::filesystem::writeFile(std::filesystem::path path, std::string value) {
    SDL_IOStream* file = SDL_IOFromFile(path.c_str(), "wb");
    if(file == nullptr) {
        TA::handleSDLError("open %s for write failed", path.c_str());
    }

    for(int pos = 0; pos < (int)value.size(); pos++) {
        SDL_WriteIO(file, &value[pos], 1);
    }

    if(!SDL_CloseIO(file)) {
        TA::handleSDLError("close %s after writing failed", path.c_str());
    }
}
