#include "geometry.h"
#include "error.h"
#include "tools.h"

TA_Point::TA_Point(double newX, double newY)
{
    x = newX;
    y = newY;
}

TA_Point TA_Point::operator+(const TA_Point &rv) const
{
    TA_Point res;
    res.x = x + rv.x;
    res.y = y + rv.y;
    return res;
}

TA_Point TA_Point::operator-(const TA_Point &rv) const
{
    TA_Point res;
    res.x = x - rv.x;
    res.y = y - rv.y;
    return res;
}

TA_Point TA_Point::operator*(const TA_Point &rv) const
{
    TA_Point res;
    res.x = x * rv.x;
    res.y = y * rv.y;
    return res;
}

TA_Point TA_Point::operator*(const double &rv) const
{
    TA_Point res;
    res.x = x * rv;
    res.y = y * rv;
    return res;
}

double TA_Point::length() const
{
    return sqrt(x * x + y * y);
}

double TA_Point::getDistance(TA_Point rv) const
{
    return sqrt((x - rv.x) * (x - rv.x) + (y - rv.y) * (y - rv.y));
}

double TA_Line::getLineEquation(TA_Point point) const
{
    double res = (point.x - first.x) * (second.y - first.y) - (point.y - first.y) * (second.x - first.x);
    return res;
}

bool TA_Line::intersects(TA_Line rv) const
{
    std::vector<double> sign(4);
    sign[0] = getLineEquation(rv.first);
    sign[1] = getLineEquation(rv.second);
    sign[2] = rv.getLineEquation(first);
    sign[3] = rv.getLineEquation(second);
    return sign[0] * sign[1] <= 0 && sign[2] * sign[3] <= 0;
}

void TA_Polygon::addVertex(TA_Point vertex)
{
    if(circle) {
        vertexList.clear();
        circle = false;
    }
    rect = false;
    vertexList.push_back(vertex);
    if(vertexList.size() == 4 && TA::equal(getVertex(1).x, getVertex(2).x) &&
       TA::equal(getVertex(1).y, getVertex(0).y) && TA::equal(getVertex(3).x, getVertex(0).x) &&
       TA::equal(getVertex(3).y, getVertex(2).y)) {
        rect = true;
    }
}

void TA_Polygon::setRectangle(TA_Point topLeft, TA_Point bottomRight)
{
    vertexList.clear();
    addVertex(topLeft);
    addVertex({bottomRight.x, topLeft.y});
    addVertex(bottomRight);
    addVertex({topLeft.x, bottomRight.y});
    rect = true;
}

void TA_Polygon::setCircle(TA_Point newVertex, double newRadius)
{
    radius = newRadius;
    circle = true;
    rect = false;
    vertexList.clear();
    vertexList.push_back(newVertex);
}

bool TA_Polygon::inside(TA_Point point)
{
    if(circle) {
        double distance = point.getDistance(vertexList[0] + position);
        return distance <= radius;
    }

    TA_Line ray;
    ray.first = point;
    ray.second = {1e5, point.y};
    int count = 0;

    for(int pos = 0; pos < int(vertexList.size()); pos ++) {
        TA_Line currentLine = {vertexList[pos] + position, vertexList[(pos + 1) % vertexList.size()] + position};
        if(ray.intersects(currentLine)) {
            count ++;
        }
    }

    return count % 2 == 1;
}

bool TA_Polygon::intersects(TA_Polygon &rv)
{
    if(empty() || rv.empty()) {
        return false;
    }

    if(isRectangle() && rv.isRectangle()) {
        return getTopLeft().x < rv.getBottomRight().x && getBottomRight().x > rv.getTopLeft().x &&
               getTopLeft().y < rv.getBottomRight().y && getBottomRight().y > rv.getTopLeft().y;
    }

    for(int pos1 = 0; pos1 < size(); pos1 ++) {
        TA_Line line1 = {getVertex(pos1), getVertex(pos1 + 1)};
        for(int pos2 = 0; pos2 < rv.size(); pos2 ++) {
            TA_Line line2 = {rv.getVertex(pos2), rv.getVertex(pos2 + 1)};
            if(line1.intersects(line2)) {
                return true;
            }
        }
    }

    if(rv.inside(getVertex(0))) {
        return true;
    }
    if(inside(rv.getVertex(0))) {
        return true;
    }
    return false;
}

TA_Point TA_Polygon::getVertex(int pos)
{
    if(empty()) {
        TA::handleError("Vertex list is empty");
    }
    return vertexList[pos % vertexList.size()] + position;
}
