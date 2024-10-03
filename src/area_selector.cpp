#include "area_selector.h"
#include "save.h"

void TA_AreaSelector::load()
{
    controller.load();
    appendPoints();
    addSelectedArea();
    setActivePoints();
    setPointNeighbours();
    loadTailsIcon();
    switchSound.load("sound/switch.ogg", TA_SOUND_CHANNEL_SFX1);
}

void TA_AreaSelector::appendPoints()
{
    double xOffset = (TA::screenWidth - 256) / 2;

    if(TA::save::getSaveParameter("seafox")) {
        points.push_back(new TA_MapPoint(0, " tails'\n house", "", TA_Point(122 + xOffset, 88)));
        points.push_back(new TA_MapPoint(1, " lake\n rocky", "maps/lr/lr1", TA_Point(131 + xOffset, 111)));
    }
    else {
        points.push_back(new TA_MapPoint(0, " tails'\n house", "", TA_Point(122 + xOffset, 97)));
        points.push_back(new TA_MapPoint(1, " poloy\n forest", "maps/pf/pf1", TA_Point(106 + xOffset, 89)));
        points.push_back(new TA_MapPoint(2, "volcanic\ntunnel", "maps/vt/vt1", TA_Point(146 + xOffset, 73)));
        points.push_back(new TA_MapPoint(3, " polly\n mt.1", "maps/pm/pm1", TA_Point(146 + xOffset, 41)));
        points.push_back(new TA_MapPoint(4, "cavern\nisland", "maps/ci/ci1", TA_Point(182 + xOffset, 33)));
        points.push_back(new TA_MapPoint(4, " caron\n forest", "maps/cf/cf1", TA_Point(106 + xOffset, 49)));
    }

    currentPoint = points[TA::save::getSaveParameter("map_selection")];
}

void TA_AreaSelector::addSelectedArea()
{
    long long areaMask = TA::save::getSaveParameter("area_mask");
    int index = TA::save::getSaveParameter("map_selection");
    if(TA::save::getSaveParameter("seafox")) {
        index += 9;
    }

    if((areaMask & (1ll << index)) == 0) {
        areaMask |= (1ll << index);
        TA::save::setSaveParameter("area_mask", areaMask);
        TA::save::setSaveParameter("last_unlocked", index);
    }
}

void TA_AreaSelector::setActivePoints()
{
    int mask = TA::save::getSaveParameter("area_mask");
    int add = (TA::save::getSaveParameter("seafox") ? 9 : 0);
    for(int level = 0; level < (int)points.size(); level ++) {
        if(mask & (1 << (level + add))) {
            points[level]->activate();
        }
    }
}

void TA_AreaSelector::setPointNeighbours()
{
    if(TA::save::getSaveParameter("seafox")) {
        points[0]->setNeighbour(TA_DIRECTION_DOWN, points[1]);
        points[1]->setNeighbour(TA_DIRECTION_UP, points[0]);
    }

    else {
        points[0]->setNeighbour(TA_DIRECTION_UP, points[1]);
        points[0]->setNeighbour(TA_DIRECTION_LEFT, points[1]);

        points[1]->setNeighbour(TA_DIRECTION_DOWN, points[0]);
        points[1]->setNeighbour(TA_DIRECTION_RIGHT, points[2]);
        points[1]->setNeighbour(TA_DIRECTION_UP, points[5]);

        points[2]->setNeighbour(TA_DIRECTION_DOWN, points[0]);
        points[2]->setNeighbour(TA_DIRECTION_LEFT, points[1]);
        points[2]->setNeighbour(TA_DIRECTION_UP, points[3]);

        points[3]->setNeighbour(TA_DIRECTION_DOWN, points[2]);
        points[3]->setNeighbour(TA_DIRECTION_RIGHT, points[4]);
        points[3]->setNeighbour(TA_DIRECTION_UP, points[4]);

        points[4]->setNeighbour(TA_DIRECTION_LEFT, points[3]);

        points[5]->setNeighbour(TA_DIRECTION_DOWN, points[1]);
    }
}

void TA_AreaSelector::loadTailsIcon()
{
    tailsIcon.load("worldmap/tails_icon.png", 8, 16);
    tailsIcon.loadAnimationsFromFile("worldmap/tails_icon.xml");
    tailsIcon.setAnimation(TA::save::getSaveParameter("seafox") ? "seafox" : "blink");
}

TA_ScreenState TA_AreaSelector::update()
{
    controller.update();
    if(controller.isJustChangedDirection()) {
        TA_Direction direction = controller.getDirection();
        if(direction != TA_DIRECTION_MAX && currentPoint->getNeighbour(direction) != nullptr
            && currentPoint->getNeighbour(direction)->isActive()) {
            currentPoint = currentPoint->getNeighbour(direction);
            switchSound.play();
        }
    }

    for(int pos = 0; pos < (int)points.size(); pos ++) {
        if(currentPoint != points[pos] && points[pos]->isActive() && points[pos]->updateButton()) {
            currentPoint = points[pos];
            switchSound.play();
        }
    }

    TA::save::setSaveParameter("map_selection", currentPoint->getIndex());
    tailsIcon.setPosition(currentPoint->getPosition() + TA_Point(-2, 8));

    if(controller.isJustPressed(TA_BUTTON_A) || controller.isJustPressed(TA_BUTTON_B) || currentPoint->updateButton()) {
        TA::levelPath = currentPoint->getPath();
        if(TA::levelPath == "") {
            return TA_SCREENSTATE_HOUSE;
        }
        return TA_SCREENSTATE_GAME;
    }
    return TA_SCREENSTATE_CURRENT;
}

void TA_AreaSelector::draw()
{
    if(!TA::save::getSaveParameter("seafox")) {
        for(int pos = 1; pos < (int)points.size(); pos ++) {
            points[pos]->draw();
        }
    }
    tailsIcon.draw();
    controller.draw();
}

std::string TA_AreaSelector::getSelectionName()
{
    return currentPoint->getName();
}

TA_AreaSelector::~TA_AreaSelector()
{
    for(TA_MapPoint* point : points) {
        delete point;
    }
}

TA_MapPoint::TA_MapPoint(int index, std::string name, std::string path, TA_Point position)
{
    this->index = index;
    this->position = position;
    this->name = name;
    this->path = path;
    for(int direction = 0; direction < TA_DIRECTION_MAX; direction ++) {
        neighbours[direction] = nullptr;
    }

    sprite.load("worldmap/selection.png");
    sprite.setPosition(position);
    button.setRectangle(position - TA_Point(4, 4), position + TA_Point(16, 16));
}

bool TA_MapPoint::updateButton()
{
    button.update();
    return button.isJustPressed();
}

void TA_MapPoint::draw()
{
    if(!active) {
        return;
    }
    timer = fmod(timer + TA::elapsedTime, lightTime * 2);
    if(timer < lightTime) {
        sprite.setAlpha(255 * timer / appearTime);
    }
    else {
        sprite.setAlpha(255 - 255 * (timer - lightTime) / appearTime);
    }
    sprite.draw();
}
