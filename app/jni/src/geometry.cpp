#include "geometry.h"

TA_Point::TA_Point(double newX, double newY)
{
    x = newX;
    y = newY;
}

TA_Point TA_Point::operator+(const TA_Point &rv)
{
    TA_Point res;
    res.x = x + rv.x;
    res.y = y + rv.y;
    return res;
}

double TA_Line::getLineEquation(TA_Point point)
{
    double res = (point.x - first.x) * (second.y - first.y) - (point.y - first.y) * (second.x - first.x);
    return res;
}

bool TA_Line::intersects(TA_Line rv)
{
    std::vector<double> sign(4);
    sign[0] = getLineEquation(rv.first);
    sign[1] = getLineEquation(rv.second);
    sign[2] = rv.getLineEquation(first);
    sign[3] = rv.getLineEquation(second);
    return sign[0] * sign[1] <= 0 && sign[2] * sign[3] <= 0;
}

void TA_Polygon::setRectangle(TA_Point topLeft, TA_Point bottomRight)
{
    addVertex(topLeft);
    addVertex({bottomRight.x, topLeft.y});
    addVertex(bottomRight);
    addVertex({topLeft.x, bottomRight.y});
}

bool TA_Polygon::inside(TA_Point point)
{
    TA_Line ray;
    ray.first = point;
    ray.second = {1e9, point.y};
    int count = 0;

    for(int pos = 0; pos < int(vertexList.size()) - 1; pos ++) {
        TA_Line currentLine = {vertexList[pos], vertexList[pos + 1]};
        if(ray.intersects(currentLine)) {
            count ++;
        }
    }

    return count % 2 == 1;
}
