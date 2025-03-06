#ifndef TA_TOOLS_H
#define TA_TOOLS_H

#include <cmath>
#include <set>
#include <string>
#include <vector>
#include "SDL3/SDL.h"
#include "geometry.h"

enum TA_Direction { TA_DIRECTION_UP, TA_DIRECTION_DOWN, TA_DIRECTION_LEFT, TA_DIRECTION_RIGHT, TA_DIRECTION_MAX };

enum TA_FunctionButton { TA_BUTTON_A, TA_BUTTON_B, TA_BUTTON_PAUSE, TA_BUTTON_LB, TA_BUTTON_RB, TA_BUTTON_MAX };

namespace TA {
    extern SDL_Window* window;
    extern SDL_Renderer* renderer;

    extern int screenWidth, screenHeight, scaleFactor;
    extern double elapsedTime;

    constexpr double pi = atan(double(1)) * 4;

    extern std::string levelPath, previousLevelPath;
    extern std::set<std::string> arguments;

    void drawRect(TA_Point topLeft, TA_Point bottomRight, int r, int g, int b, int a);
    void drawScreenRect(int r, int g, int b, int a);
    void drawShadow(int factor);
    double linearInterpolation(double left, double right, double pos);
    int getBaseHeight(int index);

    template <typename T>
    T sign(T a) {
        return (a >= 0 ? 1 : -1);
    }

    namespace random {
        void init(unsigned long long seed);
        long long next();
        long long max();
    }
}

#endif // TA_TOOLS_H
