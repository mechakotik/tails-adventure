#ifndef TA_GEOMETRY_H
#define TA_GEOMETRY_H

#include <vector>
#include <cmath>

struct TA_Point {
    double x = 0, y = 0;

    TA_Point() = default;
    TA_Point(double newX, double newY): x(newX), y(newY) {}

    TA_Point operator+(const TA_Point &rv) const {
        return {x + rv.x, y + rv.y};
    }
    TA_Point operator-(const TA_Point &rv) const {
        return {x - rv.x, y - rv.y};
    }
    TA_Point operator*(const TA_Point &rv) const {
        return {x * rv.x, y * rv.y};
    }
    TA_Point operator*(const double &rv) const {
        return {x * rv, y * rv};
    }

    [[nodiscard]] double length() const {
        return std::sqrt(x * x + y * y);
    }
    [[nodiscard]] double getDistance(TA_Point rv) const {
        return (*this - rv).length();
    }
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
    bool intersects(TA_Polygon &rv);
    int size() {return vertexList.size();}
    bool empty() {return size() == 0;}
    TA_Point getVertex(int pos);
    bool isRectangle() {return rect;}
    TA_Point getTopLeft() {return getVertex(0);}
    TA_Point getBottomRight() {return getVertex(2);}
};

#endif // TA_GEOMETRY_H
