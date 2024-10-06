#include <filesystem>
#include "SDL3/SDL.h"
#include "filesystem.h"
#include "error.h"

#ifdef _WIN32
#include "windows.h"
#elif __linux__
#include <limits.h>
#include <unistd.h>
#endif

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
    SDL_IOStream *file = SDL_IOFromFile(path.c_str(), "rb");
    if(file == nullptr) {
        return false;
    }

    if(!SDL_CloseIO(file)) {
        TA::handleSDLError("Close %s after checking existence failed", path.c_str());
    }
    return true;
}

std::string TA::filesystem::readFile(std::string path)
{
    fixPath(path);
    SDL_IOStream *file = SDL_IOFromFile(path.c_str(), "rb");
    if(file == nullptr) {
        TA::handleSDLError("Open %s for read failed", path.c_str());
    }

    size_t dataBytes = SDL_SeekIO(file, 0, SDL_IO_SEEK_END);
    SDL_SeekIO(file, 0, SDL_IO_SEEK_SET);
    char* data = new char[dataBytes];
    SDL_ReadIO(file, data, dataBytes);

    std::string str(dataBytes + 1, 0);
    for(int pos = 0; pos < dataBytes; pos ++) {
        str[pos] = data[pos];
    }
    str[dataBytes] = '\0';
    delete[] data;

    if(!SDL_CloseIO(file)) {
        TA::handleSDLError("Close %s after reading failed", path.c_str());
    }
    return str;
}

std::string TA::filesystem::readAsset(std::string path)
{
    std::string prefix = getAssetsPath();
    return readFile(prefix + "/" + path);
}

std::string TA::filesystem::getAssetsPath()
{
    #ifdef __ANDROID__
        return ".";
    #elifdef TA_UNIX_INSTALL
        return "/usr/share/tails-adventure";
    #else
        return getExecutableDirectory() + "/assets";
    #endif
}

std::string TA::filesystem::getExecutableDirectory()
{
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

void TA::filesystem::writeFile(std::string path, std::string value)
{
    fixPath(path);
    SDL_IOStream *file = SDL_IOFromFile(path.c_str(), "wb");
    if(file == nullptr) {
        TA::handleSDLError("Open %s for write failed", path.c_str());
    }

    for(int pos = 0; pos < (int)value.size(); pos ++) {
        SDL_WriteIO(file, &value[pos], 1);
    }

    if(!SDL_CloseIO(file)) {
        TA::handleSDLError("Close %s after writing failed", path.c_str());
    }
}
