#ifndef TA_TOOLS_H
#define TA_TOOLS_H

#include <string>
#include <globals.h>

std::string readStringFromFile(std::string filename);
void drawScreenRect(int r, int g, int b, int a);
void drawShadow(int factor);
bool equal(double a, double b);

#endif // TA_TOOLS_H
