#ifndef TA_AREA_SELECTOR_H
#define TA_AREA_SELECTOR_H

#include <vector>
#include "controller.h"
#include "screen_state_machine.h"
#include "sound.h"

class TA_MapPoint;

class TA_AreaSelector {
private:
    void loadPoints();
    void appendPoints();
    void fixAreaMask();
    void addSelectedArea();
    void setActivePoints();
    void loadTailsIcon();

    TA_Controller controller;
    std::vector<TA_MapPoint> points;
    TA_Sprite tailsIcon;
    TA_Sound switchSound;
    int pos;
    float bfTimer = 0;

public:
    void load();
    TA_ScreenState update();
    std::string getSelectionName();
    void draw();
};

class TA_MapPoint {
private:
    const float lightTime = 15, appearTime = 3;

    TA_Sprite sprite;
    TA_OnscreenButton button;
    TA_Point position;
    std::string name = "", path = "";
    std::array<int, TA_DIRECTION_MAX> neighbours;
    bool battleFortress;

    bool active = false;
    float bfTimer = 0;
    float lightTimer = 0;

public:
    TA_MapPoint(std::string name, std::string path, TA_Point position, bool battleFortress);
    void setNeighbour(TA_Direction direction, int neighbour) { neighbours[direction] = neighbour; }
    void activate() { active = true; }
    bool updateButton();
    void draw();

    TA_Point getPosition() { return position; }
    std::string getName() { return name; }
    std::string getPath() { return path; }
    bool isActive() { return active; }
    bool isBattleFortress() { return battleFortress; }
    int getNeighbour(TA_Direction direction) { return neighbours[direction]; }
};

#endif // TA_AREA_SELECTOR_H
