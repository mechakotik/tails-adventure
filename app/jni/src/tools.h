#ifndef TA_TOOLS_H
#define TA_TOOLS_H

#include <string>

std::string readStringFromFile(std::string filename);
void drawScreenRect(int r, int g, int b, int a);
void drawShadow(int factor);

#endif // TA_TOOLS_H
