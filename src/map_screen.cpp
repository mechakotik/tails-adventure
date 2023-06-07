#include "map_screen.h"
#include "save.h"

void TA_MapScreen::init()
{
    tilemap.load("worldmap/map.tmx");
    loadPoints();
}

void TA_MapScreen::loadPoints()
{
    appendPoints();
    setActivePoints();
    setPointNeighbours();
}

void TA_MapScreen::appendPoints()
{
    points.push_back(new TA_MapPoint("tails'\nhouse", "", TA_Point(0, 0)));
    points.push_back(new TA_MapPoint("poloy\nforest", "maps/pf/pf1", TA_Point(0, 0)));
    points.push_back(new TA_MapPoint("volcanic\ntunnel", "maps/vt/vt1", TA_Point(0, 0)));
}

void TA_MapScreen::setActivePoints()
{
    int mask = TA::save::getSaveParameter("level_mask");
    for(int level = 0; level < (int)points.size(); level ++) {
        if(mask & (1 << level)) {
            points[level]->activate();
        }
    }
}

void TA_MapScreen::setPointNeighbours()
{
    points[0]->setNeighbour(TA_DIRECTION_UP, points[1]);
    points[0]->setNeighbour(TA_DIRECTION_LEFT, points[1]);

    points[1]->setNeighbour(TA_DIRECTION_DOWN, points[0]);
    points[1]->setNeighbour(TA_DIRECTION_RIGHT, points[2]);

    points[2]->setNeighbour(TA_DIRECTION_DOWN, points[0]);
    points[2]->setNeighbour(TA_DIRECTION_LEFT, points[1]);
}

TA_ScreenState TA_MapScreen::update()
{
    tilemap.draw(0);
    return TA_SCREENSTATE_CURRENT;
}

void TA_MapScreen::quit()
{
    for(TA_MapPoint* point : points) {
        delete point;
    }
}

TA_MapPoint::TA_MapPoint(std::string name, std::string path, TA_Point position)
{
    this->position = position;
    this->name = name;
    this->path = path;
    for(int direction = 0; direction < TA_DIRECTION_MAX; direction ++) {
        neighbours[direction] = nullptr;
    }

    sprite.load("worldmap/point.png");
    sprite.setPosition(position);
}
