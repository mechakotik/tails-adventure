#include "map_screen.h"
#include "save.h"

void TA_MapScreen::init()
{
    tilemap.load("worldmap/map.tmx");
    controller.load();

    tailsIcon.load("worldmap/tails_icon.png", 7, 7);
    tailsIcon.loadAnimationsFromFile("worldmap/tails_icon.xml");
    tailsIcon.setAnimation("blink");

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
    points.push_back(new TA_MapPoint("tails'\nhouse", "", TA_Point(122, 97)));
    points.push_back(new TA_MapPoint("poloy\nforest", "maps/pf/pf1", TA_Point(106, 89)));
    points.push_back(new TA_MapPoint("volcanic\ntunnel", "maps/vt/vt1", TA_Point(146, 73)));
    currentPoint = points[0];
}

void TA_MapScreen::setActivePoints()
{
    int mask = TA::save::getSaveParameter("area_mask");
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
    handleInput();
    draw();

    if(controller.isJustPressed(TA_BUTTON_A) || controller.isJustPressed(TA_BUTTON_B)) {
        TA::levelPath = currentPoint->getPath();
        return TA_SCREENSTATE_GAME;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_MapScreen::handleInput()
{
    controller.update();
    if(controller.isJustChangedDirection()) {
        TA_Direction direction = controller.getDirection();
        if(direction != TA_DIRECTION_MAX && currentPoint->getNeighbour(direction) != nullptr) {
            currentPoint = currentPoint->getNeighbour(direction);
        }
    }
    tailsIcon.setPosition(currentPoint->getPosition() + TA_Point(-2, 8));
}

void TA_MapScreen::draw()
{
    tilemap.draw(0);
    for(int pos = 1; pos < (int)points.size(); pos ++) {
        points[pos]->draw();
    }
    tailsIcon.draw();
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

    sprite.load("worldmap/selection.png");
    sprite.setPosition(position);
}

void TA_MapPoint::draw()
{
    if(!active) {
        return;
    }
    timer = std::fmod(timer + TA::elapsedTime, lightTime * 2);
    if(timer < lightTime) {
        sprite.setAlpha(255 * timer / appearTime);
    }
    else {
        sprite.setAlpha(255 - 255 * (timer - lightTime) / appearTime);
    }
    sprite.draw();
}
