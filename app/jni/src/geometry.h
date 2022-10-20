#ifndef TA_GEOMETRY_H
#define TA_GEOMETRY_H

#include <vector>

struct TA_Point {
    double x = 0, y = 0;
    TA_Point() {}
    TA_Point(double newX, double newY);
    TA_Point operator+(const TA_Point &rv);
    TA_Point operator-(const TA_Point &rv);
    TA_Point operator*(const TA_Point &rv);
};

struct TA_Rect {
    TA_Point topLeft, bottomRight;
};

struct TA_Line {
    TA_Point first, second;
    double getLineEquation(TA_Point point);
    bool intersects(TA_Line rv);
};

class TA_Polygon {
private:
    std::vector<TA_Point> vertexList;
    bool circle = false;
    double radius = 0;

public:
    TA_Polygon() {}
    void setRectangle(TA_Point topLeft, TA_Point bottomRight);
    void setCircle(TA_Point newVertex, double newRadius);
    void addVertex(TA_Point vertex);
    bool inside(TA_Point point);
};

#endif // TA_GEOMETRY_H
