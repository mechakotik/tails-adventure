#include <algorithm>
#include <vector>
#include <limits>
#include <fstream>
#include "SDL2/SDL.h"
#include "tools.h"
#include "error.h"
#include "save.h"

namespace TA
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    int screenWidth, screenHeight, widthMultiplier, heightMultiplier;
    double elapsedTime;

    std::string levelPath = "", previousLevelPath = "";

    namespace random
    {
        unsigned long long x = 1;
    }

    namespace eventLog
    {
        std::ifstream input;
        std::ofstream output;
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
    char* data = new char[dataBytes];
    SDL_RWread(input, data, 1, dataBytes);

    std::string str(dataBytes + 1, 0);
    for(int pos = 0; pos < dataBytes; pos ++) {
        str[pos] = data[pos];
    }
    str += '\0';
    delete[] data;
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
    rect.w = rect.h = 10000;

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

double TA::linearInterpolation(double left, double right, double pos)
{
    pos = fmod(pos, 2);
    if(pos < 1) {
        return left + (right - left) / pos;
    }
    return right - (right - left) / (pos - 1);
}

void TA::eventLog::openRead(std::string filename)
{
    input.open(filename);
}

void TA::eventLog::openWrite(std::string filename)
{
    output.open(filename, std::ofstream::trunc);
}

bool TA::eventLog::isReading()
{
    return input.is_open();
}

bool TA::eventLog::isWriting()
{
    return output.is_open();
}

long long TA::eventLog::read()
{
    long long x;
    input >> x;
    return x;
}

void TA::eventLog::write(long long x)
{
    output << x << ' ';
}

void TA::eventLog::quit()
{
    if(input.is_open()) {
        input.close();
    }
    if(output.is_open()) {
        output.close();
    }
}

int TA::getFPSLimit()
{
    int factor = TA::save::getParameter("fps_limit");
    if(factor == 0) {
        return 0;
    }

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    
    int refreshRate = displayMode.refresh_rate;
    if(refreshRate == 0) {
        refreshRate = 60;
    }
    return refreshRate * factor / 2;
}
