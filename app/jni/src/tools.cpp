#include <algorithm>
#include "SDL.h"
#include "tools.h"
#include "error.h"

namespace TA
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_DisplayMode displayMode;

    int screenWidth, screenHeight;
    double elapsedTime, widthMultiplier, heightMultiplier;

    std::string levelPath = "maps/pf/pf2", previousLevelPath = "";

    namespace random
    {
        unsigned long long x = 1;
    }
}

std::string TA::readStringFromFile(std::string filename)
{
    TA::addPathPrefix(filename);
    SDL_RWops *input = SDL_RWFromFile(filename.c_str(), "r+b");
    if(input == nullptr) {
        TA::handleSDLError("Open %s failed", filename.c_str());
    }
    int dataBytes = SDL_RWseek(input, 0, SEEK_END);
    SDL_RWseek(input, 0, SEEK_SET);
    char* data = (char*)malloc(dataBytes);
    SDL_RWread(input, data, 1, dataBytes);

    std::string str(dataBytes + 1, 0);
    for(int pos = 0; pos < dataBytes; pos ++) {
        str[pos] = data[pos];
    }
    str += '\0';
    if(SDL_RWclose(input) != 0) {
        TA::handleSDLError("Close %s failed", filename.c_str());
    }
    return str;
}

void TA::drawScreenRect(int r, int g, int b, int a)
{
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = displayMode.w;
    rect.h = displayMode.h;

    a = std::max(a, 0);
    a = std::min(a, 255);
    SDL_SetRenderDrawColor(TA::renderer, r, g, b, a);
    SDL_RenderFillRect(TA::renderer, &rect);
}

void TA::drawShadow(int factor)
{
    drawScreenRect(0, 0, 0, factor);
}

bool TA::equal(double a, double b)
{
    return std::abs(a - b) < TA::epsilon;
}

void TA::random::init(unsigned long long seed)
{
    seed = std::max(seed, 1ull);
    x = seed;
}

long long TA::random::next()
{
    x ^= (x << 13);
    x ^= (x >> 7);
    x ^= (x << 17);
    return (long long)(x % max());
}

long long TA::random::max()
{
    return std::numeric_limits<long long>::max();
}

void TA::addPathPrefix(std::string &path)
{
    #ifndef __ANDROID__
    const std::string prefix = "assets/";
    if(path.length() < prefix.length() || path.substr(0, prefix.length()) != prefix) {\
        path = prefix + path;
    }
    #endif
}
