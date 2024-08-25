#include <algorithm>
#include <vector>
#include <limits>
#include <fstream>
#include "SDL.h"
#include "tools.h"
#include "error.h"
#include "save.h"

namespace TA
{
    SDL_Window *window;
    SDL_Renderer *renderer;

    int screenWidth, screenHeight, scaleFactor;
    double elapsedTime;

    std::string levelPath = "", previousLevelPath = "";
    std::set<std::string> arguments;

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
    int value = TA::save::getParameter("fps_limit");
    if(value == 0) {
        return 0;
    }
    return value + 29;
}
