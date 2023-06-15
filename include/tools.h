#ifndef TA_TOOLS_H
#define TA_TOOLS_H

#include <string>
#include <SDL.h>

enum TA_Direction {
    TA_DIRECTION_UP,
    TA_DIRECTION_DOWN,
    TA_DIRECTION_LEFT,
    TA_DIRECTION_RIGHT,
    TA_DIRECTION_MAX
};

enum TA_FunctionButton {
    TA_BUTTON_A,
    TA_BUTTON_B,
    TA_BUTTON_PAUSE,
    TA_BUTTON_LB,
    TA_BUTTON_RB,
    TA_BUTTON_MAX
};

namespace TA
{
    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
    extern SDL_DisplayMode displayMode;

    extern int screenWidth, screenHeight, widthMultiplier, heightMultiplier;
    extern double elapsedTime;
    const double epsilon = 1e-5;

    extern std::string levelPath, previousLevelPath;

    std::string readStringFromFile(std::string filename);
    void drawScreenRect(int r, int g, int b, int a);
    void drawShadow(int factor);
    bool equal(double a, double b);
    void addPathPrefix(std::string &path);
    double linearInterpolation(double left, double right, double pos);

    template<typename T>
    T sign(T a) {
        return (a >= 0 ? 1 : -1);
    }

    namespace random
    {
        void init(unsigned long long seed);
        long long next();
        long long max();
    }
}

#endif // TA_TOOLS_H
