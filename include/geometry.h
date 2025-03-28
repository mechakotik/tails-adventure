#ifndef TA_GEOMETRY_H
#define TA_GEOMETRY_H

#include <cmath>
#include <cstddef>
#include <eve/module/core.hpp>
#include <eve/wide.hpp>
#include <vector>

namespace TA {
    constexpr float epsilon = 0.001;

    inline bool equal(const float& a, const float& b) {
        return std::abs(a - b) < epsilon;
    }
}

struct TA_Point {
    float x = 0, y = 0;

    TA_Point() = default;

    template <typename T1, typename T2>
    TA_Point(const T1& x, const T2& y) : x(x), y(y) {}

    TA_Point operator+(const TA_Point& rv) const { return {x + rv.x, y + rv.y}; }
    TA_Point operator-(const TA_Point& rv) const { return {x - rv.x, y - rv.y}; }
    TA_Point operator*(const TA_Point& rv) const { return {x * rv.x, y * rv.y}; }
    TA_Point operator*(const float& rv) const { return {x * rv, y * rv}; }

    [[nodiscard]] float length() const { return std::sqrt((x * x) + (y * y)); }
    [[nodiscard]] float getDistance(const TA_Point& rv) const { return (*this - rv).length(); }
};

class TA_Rect {
public:
    TA_Rect() {
        std::fill(src.begin(), src.end(), 0);
        std::fill(pos.begin(), pos.end(), 0);
        std::fill(data.begin(), data.end(), 0);
    }

    TA_Rect(TA_Point topLeft, TA_Point bottomRight) {
        pos[0] = pos[1] = pos[2] = pos[3] = 0;
        setRectangle(topLeft, bottomRight);
    }

    void setRectangle(TA_Point topLeft, TA_Point bottomRight) {
        src[0] = topLeft.x;
        src[1] = topLeft.y;
        src[2] = bottomRight.x;
        src[3] = bottomRight.y;
        updateData();
    }

    void setPosition(TA_Point position) {
        pos[0] = pos[2] = position.x;
        pos[1] = pos[3] = position.y;
        updateData();
    }

    [[nodiscard]] TA_Point getTopLeft() const { return {data[0], data[1]}; }

    [[nodiscard]] TA_Point getBottomRight() const { return {data[2], data[3]}; }

    [[nodiscard]] bool intersects(const TA_Rect& rv) const {
        eve::wide<float, eve::fixed<4>> v1{data.data()};
        eve::wide<float, eve::fixed<4>> v2 =
            eve::gather(rv.data.data(), eve::wide<unsigned char, eve::fixed<4>>{2, 3, 0, 1});
        eve::logical<eve::wide<float, eve::fixed<4>>> mask{1, 1, 0, 0};
        return eve::all((v1 < v2) == mask);
    }

    // kept for compatibility

    [[nodiscard]] bool inside(TA_Point pt) const {
        return data[0] <= pt.x && pt.x <= data[2] && data[1] <= pt.y && pt.y <= data[3];
    }

    [[nodiscard]] TA_Point getVertex(int pos) const {
        switch(pos) {
            case 0:
                return {data[0], data[1]};
            case 1:
                return {data[2], data[1]};
            case 2:
                return {data[2], data[3]};
            case 3:
                return {data[0], data[3]};
            default:
                return {data[0], data[2]};
        }
    }

private:
    void updateData() {
        eve::wide<float, eve::fixed<4>> srcVector{src.data()};
        eve::wide<float, eve::fixed<4>> posVector{pos.data()};
        eve::wide<float, eve::fixed<4>> dataVector = srcVector + posVector;
        eve::store(dataVector, data.data());
    }

    std::array<float, 4> src, pos, data;
};

class TA_Line {
public:
    TA_Line(const TA_Point& p1, const TA_Point& p2)
        : p1(p1), p2(p2), a(p2.y - p1.y), b(p1.x - p2.x), c((p2.x * p1.y) - (p1.x * p2.y)) {
        const float k = (c > 0 ? -1.0 : 1.0) / std::sqrt((a * a) + (b * b));
        a *= k;
        b *= k;
        c *= k;
    }

    [[nodiscard]] const TA_Point& getFirst() const { return p1; }
    [[nodiscard]] const TA_Point& getSecond() const { return p2; }

    [[nodiscard]] float getDeviation(const TA_Point& point) const { return (a * point.x) + (b * point.y) + c; }

    [[nodiscard]] float getDistance(const TA_Point& point) const { return std::abs(getDeviation(point)); }

    [[nodiscard]] bool intersects(const TA_Line& rv) const {
        const float s1 = getDeviation(rv.getFirst());
        const float s2 = getDeviation(rv.getSecond());
        const float s3 = rv.getDeviation(getFirst());
        const float s4 = rv.getDeviation(getSecond());
        return s1 * s2 <= 0 && s3 * s4 <= 0;
    }

private:
    TA_Point p1, p2;
    float a = 0, b = 0, c = 0;
};

class TA_Polygon {
public:
    TA_Polygon() = default;

    TA_Polygon(const TA_Point& topLeft, const TA_Point& bottomRight) { setRectangle(topLeft, bottomRight); }

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

    [[nodiscard]] const TA_Point& getPosition() const { return position; }

    void addVertex(const TA_Point& vertex) {
        sourceVertexList.push_back(vertex);
        updateVertexList();

        rect = (vertexList.size() == 4 && TA::equal(getVertex(1).x, getVertex(2).x) &&
                TA::equal(getVertex(1).y, getVertex(0).y) && TA::equal(getVertex(3).x, getVertex(0).x) &&
                TA::equal(getVertex(3).y, getVertex(2).y));
    }

    [[nodiscard]] bool inside(const TA_Point& point) const {
        if(isRectangle()) [[likely]] {
            return getTopLeft().x <= point.x && point.x <= getBottomRight().x && getTopLeft().y <= point.y &&
                   point.y <= getBottomRight().y;
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

    [[nodiscard]] bool intersects(const TA_Rect& rv) const {
        if(isRectangle()) [[likely]] {
            return getTopLeft().x < rv.getBottomRight().x && getBottomRight().x > rv.getTopLeft().x &&
                   getTopLeft().y < rv.getBottomRight().y && getBottomRight().y > rv.getTopLeft().y;
        }

        if(empty()) [[unlikely]] {
            return false;
        }

        for(int pos1 = 0; pos1 < size(); pos1++) {
            const TA_Line line1 = {getVertex(pos1), getVertex((pos1 + 1) % size())};
            for(int pos2 = 0; pos2 < 4; pos2++) {
                const TA_Line line2 = {rv.getVertex(pos2), rv.getVertex((pos2 + 1) % 4)};
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

    [[nodiscard]] size_t size() const { return vertexList.size(); }
    [[nodiscard]] bool empty() const { return size() == 0; }
    [[nodiscard]] bool isRectangle() const { return rect; }

    [[nodiscard]] const TA_Point& getVertex(const size_t& pos) const { return vertexList[pos]; }
    [[nodiscard]] const TA_Point& getTopLeft() const { return getVertex(0); }
    [[nodiscard]] const TA_Point& getBottomRight() const { return getVertex(2); }

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

    TA_Circle(const TA_Point& center, const float& radius) : center(center), radius(radius) {}

    void setCenter(const TA_Point& center) { this->center = center; }

    [[nodiscard]] bool inside(const TA_Point& point) const { return center.getDistance(point) <= radius; }

private:
    TA_Point center;
    float radius = 0;
};

class TA_Shape {
public:
    void setPolygon(const TA_Polygon& polygon) {
        this->polygon = polygon;
        isCircle = false;
    }

    void setCircle(const TA_Circle& circle) {
        this->circle = circle;
        isCircle = true;
    }

    void setPosition(const TA_Point& point) {
        if(isCircle) {
            circle.setCenter(point);
        } else {
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
