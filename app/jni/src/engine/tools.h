#ifndef TA_TOOLS_H
#define TA_TOOLS_H

#include <string>
#include <SDL.h>

namespace TA
{
    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
    extern SDL_DisplayMode displayMode;

    extern int screenWidth, screenHeight;
    extern double elapsedTime, widthMultiplier, heightMultiplier;
    const double epsilon = 1e-5;

    std::string readStringFromFile(std::string filename);
    void drawScreenRect(int r, int g, int b, int a);
    void drawShadow(int factor);
    bool equal(double a, double b);
}

#endif // TA_TOOLS_H
