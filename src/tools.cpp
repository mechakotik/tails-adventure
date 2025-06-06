#include "tools.h"
#include <algorithm>
#include <fstream>
#include <limits>
#include <random>
#include <vector>
#include "SDL3/SDL.h"

namespace TA {
    SDL_Window* window;
    SDL_Renderer* renderer;

    int screenWidth, screenHeight, scaleFactor;
    float elapsedTime;

    std::string levelPath = "", previousLevelPath = "";
    std::set<std::string> arguments;

    namespace random {
        std::mt19937_64 gen;
    }

    namespace eventLog {
        std::ifstream input;
        std::ofstream output;
    }
}

void TA::drawRect(TA_Point topLeft, TA_Point bottomRight, int r, int g, int b, int a) {
    SDL_FRect rect;
    rect.x = topLeft.x * TA::scaleFactor;
    rect.y = topLeft.y * TA::scaleFactor;
    rect.w = (bottomRight.x - topLeft.x) * TA::scaleFactor;
    rect.h = (bottomRight.y - topLeft.y) * TA::scaleFactor;

    a = std::max(a, 0);
    a = std::min(a, 255);
    SDL_SetRenderDrawColor(TA::renderer, r, g, b, a);
    SDL_RenderFillRect(TA::renderer, &rect);
}

void TA::drawScreenRect(int r, int g, int b, int a) {
    drawRect(TA_Point(-16, -16), TA_Point(TA::screenWidth + 16, TA::screenHeight + 16), r, g, b, a);
}

void TA::drawShadow(int factor) {
    drawScreenRect(0, 0, 0, factor);
}

void TA::random::init(unsigned long long seed) {
    gen = std::mt19937_64(seed);
}

long long TA::random::next() {
    return static_cast<long long>(gen() & ~(1ULL << 63));
}

long long TA::random::max() {
    return std::numeric_limits<long long>::max();
}

float TA::linearInterpolation(float left, float right, float pos) {
    pos = fmod(pos, 2);
    if(pos < 1) {
        return left + (right - left) * pos;
    }
    return right - (right - left) * (pos - 1);
}

int TA::getBaseHeight(int index) {
    switch(index) {
        case 0:
            return 144;
        case 1:
            return 160;
        case 2:
            return 180;
        case 3:
            return 192;
        default:
            return 144;
    }
}
