#ifndef TA_TOOLS_H
#define TA_TOOLS_H

#include <string>
#include <vector>
#include <set>
#include "SDL2/SDL.h"

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

    extern int screenWidth, screenHeight, scaleFactor;
    extern double elapsedTime;

    const double pi = atan(double(1)) * 4;
    const double epsilon = 1e-5;

    extern std::string levelPath, previousLevelPath;
    extern std::set<std::string> arguments;

    std::string readStringFromFile(std::string filename);
    void drawScreenRect(int r, int g, int b, int a);
    void drawShadow(int factor);
    bool equal(double a, double b);
    void addPathPrefix(std::string &path);
    double linearInterpolation(double left, double right, double pos);
    int getFPSLimit();

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

    namespace eventLog
    {
        void openRead(std::string filename);
        void openWrite(std::string filename);
        bool isReading();
        bool isWriting();
        void write(long long x);
        void quit();
        long long read();
    }
}

#endif // TA_TOOLS_H
