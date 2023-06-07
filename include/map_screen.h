#ifndef TA_MAP_SCREEN_H
#define TA_MAP_SCREEN_H

#include "screen.h"
#include "tilemap.h"
#include "controller.h"

class TA_MapPoint;

class TA_MapScreen : public TA_Screen {
private:
    void loadPoints();
    void appendPoints();
    void setActivePoints();
    void setPointNeighbours();

    TA_Tilemap tilemap;
    std::vector<TA_MapPoint*> points;

public:
    void init() override;
    TA_ScreenState update() override;
    void quit() override;
};

class TA_MapPoint {
private:
    TA_Sprite sprite;
    TA_Point position;
    std::string name = "", path = "";
    std::array<TA_MapPoint*, TA_DIRECTION_MAX> neighbours;

public:
    TA_MapPoint(std::string name, std::string path, TA_Point position);
    void setNeighbour(TA_Direction direction, TA_MapPoint* neighbour) {neighbours[direction] = neighbour;}
    void activate() {sprite.setAnimation("active");}
    void draw() {sprite.draw();}

    TA_Point getPosition() {return position;}
    std::string getName() {return name;}
    bool isActive() {return sprite.isAnimated();}
    TA_MapPoint* getNeighbour(TA_Direction direction) {return neighbours[direction];}
};

#endif // TA_MAP_SCREEN_H
