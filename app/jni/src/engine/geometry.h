#ifndef TA_GEOMETRY_H
#define TA_GEOMETRY_H

#include <vector>

struct TA_Point {
    double x = 0, y = 0;
    TA_Point() {}
    TA_Point(double newX, double newY);
    TA_Point operator+(const TA_Point &rv) const;
    TA_Point operator-(const TA_Point &rv) const;
    TA_Point operator*(const TA_Point &rv) const;
    double length() const;
    double getDistance(TA_Point rv) const;
};

struct TA_Rect {
    TA_Point topLeft, bottomRight;
};

struct TA_Line {
    TA_Point first, second;
    double getLineEquation(TA_Point point) const;
    bool intersects(TA_Line rv) const;
};

class TA_Polygon {
private:
    std::vector<TA_Point> vertexList;
    TA_Point position;
    bool circle = false, rect = false;
    double radius = 0;

public:
    TA_Polygon() {}
    void setRectangle(TA_Point topLeft, TA_Point bottomRight);
    void setCircle(TA_Point newVertex, double newRadius);
    void setPosition(TA_Point newPosition) {position = newPosition;}
    TA_Point getPosition() {return position;}
    void addVertex(TA_Point vertex);
    bool inside(TA_Point point);
    bool intersects(TA_Polygon rv);
    int size() {return vertexList.size();}
    bool empty() {return size() == 0;}
    double getDistance(TA_Polygon rv);
    TA_Point getVertex(int pos);
    bool isRectangle() {return rect;}
    TA_Point getTopLeft() {return getVertex(0);}
    TA_Point getBottomRight() {return getVertex(2);}
};

#endif // TA_GEOMETRY_H
