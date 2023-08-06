#ifndef TA_AREA_SELECTOR_H
#define TA_AREA_SELECTOR_H

#include <vector>
#include "screen_state_machine.h"
#include "controller.h"
#include "sound.h"

class TA_MapPoint;

class TA_AreaSelector {
private:
    void loadPoints();
    void appendPoints();
    void addSelectedArea();
    void setActivePoints();
    void setPointNeighbours();
    void loadTailsIcon();

    TA_Controller controller;
    std::vector<TA_MapPoint*> points;
    TA_Sprite tailsIcon;
    TA_MapPoint* currentPoint;
    TA_Sound switchSound;

public:
    void load();
    TA_ScreenState update();
    std::string getSelectionName();
    void draw();
    ~TA_AreaSelector();
};

class TA_MapPoint {
private:
    const double lightTime = 15, appearTime = 3;

    TA_Sprite sprite;
    TA_Point position;
    std::string name = "", path = "";
    std::array<TA_MapPoint*, TA_DIRECTION_MAX> neighbours;

    bool active = false;
    double timer = 0;

public:
    TA_MapPoint(std::string name, std::string path, TA_Point position);
    void setNeighbour(TA_Direction direction, TA_MapPoint* neighbour) {neighbours[direction] = neighbour;}
    void activate() {active = true;}
    void draw();

    TA_Point getPosition() {return position;}
    std::string getName() {return name;}
    std::string getPath() {return path;}
    bool isActive() {return active;}
    TA_MapPoint* getNeighbour(TA_Direction direction) {return neighbours[direction];}
};

#endif // TA_AREA_SELECTOR_H
