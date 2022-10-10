#include <algorithm>
#include <SDL.h>
#include "tools.h"
#include "error.h"
#include "globals.h"

std::string readStringFromFile(std::string filename)
{
    SDL_RWops *input = SDL_RWFromFile(filename.c_str(), "r+b");
    if(input == nullptr) {
        handleSDLError("Open %s failed", filename.c_str());
    }
    int data_bytes = SDL_RWseek(input, 0, SEEK_END);
    SDL_RWseek(input, 0, SEEK_SET);
    char* data = (char*)malloc(data_bytes);
    SDL_RWread(input, data, 1, data_bytes);

    std::string str(data_bytes + 1, 0);
    for(int pos = 0; pos < data_bytes; pos ++) {
        str[pos] = data[pos];
    }
    str += '\0';
    return str;
}

void drawScreenRect(int r, int g, int b, int a)
{
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    SDL_Rect rect;
    rect.x = rect.y = 0;
    rect.w = displayMode.w;
    rect.h = displayMode.h;

    a = std::max(a, 0);
    a = std::min(a, 255);
    SDL_SetRenderDrawColor(gRenderer, r, g, b, a);
    SDL_RenderFillRect(gRenderer, &rect);
}

void drawShadow(int factor)
{
    drawScreenRect(0, 0, 0, factor);
}
