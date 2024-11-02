#ifndef TA_GEOMETRY_H
#define TA_GEOMETRY_H

#include <vector>
#include <cmath>
#include <cstddef>

namespace TA {
    constexpr double epsilon = 1e-5;

    inline bool equal(const double& a, const double& b) {
        return std::abs(a - b) < epsilon;
    }
}

struct TA_Point {
    double x = 0, y = 0;

    TA_Point() = default;

    template<typename T1, typename T2>
    TA_Point(const T1& x, const T2& y): x(x), y(y) {}

    TA_Point operator+(const TA_Point& rv) const {
        return {x + rv.x, y + rv.y};
    }
    TA_Point operator-(const TA_Point& rv) const {
        return {x - rv.x, y - rv.y};
    }
    TA_Point operator*(const TA_Point& rv) const {
        return {x * rv.x, y * rv.y};
    }
    TA_Point operator*(const double& rv) const {
        return {x * rv, y * rv};
    }

    [[nodiscard]] double length() const {
        return std::sqrt((x * x) + (y * y));
    }
    [[nodiscard]] double getDistance(const TA_Point& rv) const {
        return (*this - rv).length();
    }
};

struct TA_Rect {
    TA_Point topLeft, bottomRight;
};

class TA_Line {
public:
    TA_Line(const TA_Point& p1, const TA_Point& p2)
        :p1(p1), p2(p2), a(p2.y - p1.y), b(p1.x - p2.x),
        c((p2.x * p1.y) - (p1.x * p2.y)) {
        const double k = (c > 0 ? -1.0 : 1.0) / std::sqrt((a * a) + (b * b));
        a *= k;
        b *= k;
        c *= k;
    }

    [[nodiscard]] const TA_Point& getFirst() const {return p1;}
    [[nodiscard]] const TA_Point& getSecond() const {return p2;}

    [[nodiscard]] double getDeviation(const TA_Point& point) const {
        return (a * point.x) + (b * point.y) + c;
    }

    [[nodiscard]] double getDistance(const TA_Point& point) const {
        return std::abs(getDeviation(point));
    }

    [[nodiscard]] bool intersects(const TA_Line& rv) const {
        const double s1 = getDeviation(rv.getFirst());
        const double s2 = getDeviation(rv.getSecond());
        const double s3 = rv.getDeviation(getFirst());
        const double s4 = rv.getDeviation(getSecond());
        return s1 * s2 <= 0 && s3 * s4 <= 0;
    }

private:
    TA_Point p1, p2;
    double a = 0, b = 0, c = 0;
};

class TA_Polygon {
public:
    TA_Polygon() = default;

    TA_Polygon(const TA_Point& topLeft, const TA_Point& bottomRight) {
        setRectangle(topLeft, bottomRight);
    }

    void setRectangle(const TA_Point& topLeft, const TA_Point& bottomRight) {
        sourceVertexList.clear();
        sourceVertexList.emplace_back(topLeft);
        sourceVertexList.emplace_back(bottomRight.x, topLeft.y);
        sourceVertexList.emplace_back(bottomRight);
        sourceVertexList.emplace_back(topLeft.x, bottomRight.y);
        rect = true;
        updateVertexList();
    }

    void setPosition(const TA_Point& newPosition) {
        position = newPosition;
        updateVertexList();
    }

    [[nodiscard]] const TA_Point& getPosition() const {return position;}

    void addVertex(const TA_Point& vertex) {
        sourceVertexList.push_back(vertex);
        updateVertexList();

        rect = (vertexList.size() == 4 &&
            TA::equal(getVertex(1).x, getVertex(2).x) &&
            TA::equal(getVertex(1).y, getVertex(0).y) &&
            TA::equal(getVertex(3).x, getVertex(0).x) &&
            TA::equal(getVertex(3).y, getVertex(2).y));
    }

    [[nodiscard]] bool inside(const TA_Point& point) const {
        if(isRectangle()) [[likely]] {
            return getTopLeft().x <= point.x && point.x <= getBottomRight().x &&
                   getTopLeft().y <= point.y && point.y <= getBottomRight().y;
        }

        const TA_Line ray{point, {1e5, point.y}};
        int count = 0;

        for(size_t pos = 0; pos < vertexList.size(); pos += 1) {
            const TA_Line currentLine{vertexList[pos], vertexList[(pos + 1) % vertexList.size()]};
            if(ray.intersects(currentLine)) {
                count += 1;
            }
        }

        return count % 2 == 1;
    }

    [[nodiscard]] bool intersects(const TA_Polygon& rv) const {
        if(empty() || rv.empty()) [[unlikely]] {
            return false;
        }

        if(isRectangle() && rv.isRectangle()) [[likely]] {
            return getTopLeft().x < rv.getBottomRight().x && getBottomRight().x > rv.getTopLeft().x &&
                   getTopLeft().y < rv.getBottomRight().y && getBottomRight().y > rv.getTopLeft().y;
        }

        for(int pos1 = 0; pos1 < size(); pos1 ++) {
            const TA_Line line1 = {getVertex(pos1), getVertex((pos1 + 1) % size())};
            for(int pos2 = 0; pos2 < rv.size(); pos2 ++) {
                const TA_Line line2 = {rv.getVertex(pos2), rv.getVertex((pos2 + 1) % rv.size())};
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

    [[nodiscard]] size_t size() const {return vertexList.size();}
    [[nodiscard]] bool empty() const {return size() == 0;}
    [[nodiscard]] bool isRectangle() const {return rect;}

    [[nodiscard]] const TA_Point& getVertex(const size_t& pos) const {
        return vertexList[pos];
    }
    [[nodiscard]] const TA_Point& getTopLeft() const {
        return getVertex(0);
    }
    [[nodiscard]] const TA_Point& getBottomRight() const {
        return getVertex(2);
    }

private:
    std::vector<TA_Point> vertexList;
    std::vector<TA_Point> sourceVertexList;
    TA_Point position;
    bool rect = false;

    void updateVertexList() {
        vertexList = sourceVertexList;
        for(TA_Point& vertex : vertexList) {
            vertex = vertex + position;
        }
    }
};

class TA_Circle {
public:
    TA_Circle() = default;

    TA_Circle(const TA_Point &center, const double &radius)
        :center(center), radius(radius) {}

    void setCenter(const TA_Point &center) {
        this->center = center;
    }

    [[nodiscard]] bool inside(const TA_Point &point) const {
        return center.getDistance(point) <= radius;
    }

private:
    TA_Point center;
    double radius = 0;
};

class TA_Shape {
public:
    void setPolygon(const TA_Polygon &polygon) {
        this->polygon = polygon;
        isCircle = false;
    }

    void setCircle(const TA_Circle &circle) {
        this->circle = circle;
        isCircle = true;
    }

    void setPosition(const TA_Point& point) {
        if(isCircle) {
            circle.setCenter(point);
        }
        else {
            polygon.setPosition(point);
        }
    }

    void setRectangle(const TA_Point& topLeft, const TA_Point& bottomRight) {
        polygon.setRectangle(topLeft, bottomRight);
        isCircle = false;
    }

    [[nodiscard]] bool inside(const TA_Point& point) const {
        if(isCircle) {
            return circle.inside(point);
        }
        return polygon.inside(point);
    }

private:
    TA_Polygon polygon;
    TA_Circle circle;
    bool isCircle = false;
};

#endif // TA_GEOMETRY_H
