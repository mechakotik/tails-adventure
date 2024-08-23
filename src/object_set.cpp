#include "object_set.h"
#include "objects/explosion.h"
#include "objects/bomb.h"
#include "objects/breakable_block.h"
#include "objects/particle.h"
#include "objects/ring.h"
#include "objects/walker.h"
#include "objects/hover_pod.h"
#include "objects/dead_kukku.h"
#include "objects/pushable_object.h"
#include "error.h"
#include "filesystem.h"
#include "resource_manager.h"
#include "save.h"
#include "sea_fox.h"
#include "tinyxml2.h"
#include "hud.h"
#include "objects/transition.h"
#include "objects/bridge.h"
#include "objects/bird_walker.h"
#include "character.h"
#include "objects/bat_robot.h"
#include "objects/nezu.h"
#include "objects/flame.h"
#include "objects/fire.h"
#include "objects/item_box.h"
#include "objects/drill_mole.h"
#include "objects/moving_platform.h"
#include "objects/bomb_thrower.h"
#include "objects/rock_thrower.h"
#include "objects/jumper.h"
#include "objects/wind.h"
#include "objects/speedy.h"
#include "objects/mecha_golem.h"
#include "objects/grass_block.h"
#include "objects/mini_sub.h"
#include "objects/enemy_mine.h"
#include "objects/conveyor_belt.h"

TA_Object::TA_Object(TA_ObjectSet *newObjectSet)
{
    objectSet = newObjectSet;
    setCamera(objectSet->getLinks().camera);
}

void TA_Object::updatePosition()
{
    setPosition(position);
    hitbox.setPosition(position);
    for(auto & element : hitboxVector) {
        element.hitbox.setPosition(position);
    }
}

TA_Point TA_Object::getDistanceToCharacter()
{
    TA_Point characterPosition = objectSet->getCharacterPosition();
    TA_Point centeredPosition = position + TA_Point(getWidth() / 2, getHeight() / 2);
    return characterPosition - centeredPosition;
}

void TA_ObjectSet::load(std::string filename)
{
    tinyxml2::XMLDocument file;
    file.Parse(TA::resmgr::loadAsset(filename).c_str());

    for(tinyxml2::XMLElement *element = file.FirstChildElement("objects")->FirstChildElement("object");
        element != nullptr; element = element->NextSiblingElement("object"))
    {
        std::string name = element->Attribute("name");
        if(element->IntAttribute("tile_x")) {
            element->SetAttribute("x", element->IntAttribute("tile_x") * 16);
        }
        if(element->IntAttribute("tile_y")) {
            element->SetAttribute("y", element->IntAttribute("tile_y") * 16);
        }

        if(name == "breakable_block") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            bool dropsRing = false;
            if(element->Attribute("drops_ring", "true")) {
                dropsRing = true;
            }
            std::string path = "maps/pf/pf_block.png", particlePath = "maps/pf/pf_rock.png";
            if(element->Attribute("path")) {
                path = element->Attribute("path");
            }
            if(element->Attribute("particle_path")) {
                particlePath = element->Attribute("particle_path");
            }
            spawnObject<TA_BreakableBlock>(path, particlePath, position, dropsRing);
        }

        else if(name == "walker" || name == "hover_pod") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            int range = 0;
            bool direction = true;
            if (element->Attribute("range")) {
                range = element->IntAttribute("range");
            }
            if (element->Attribute("direction", "right")) {
                direction = false;
            }
            if(name == "walker") {
                spawnObject<TA_Walker>(position, range, direction);
            }
            else {
                spawnObject<TA_HoverPod>(position, range, direction);
            }
        }

        else if(name == "pushable_rock" || name == "pushable_spring") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            if(name == "pushable_rock") {
                spawnObject<TA_PushableRock>(position);
            }
            else {
                spawnObject<TA_PushableSpring>(position);
            }
        }

        else if(name == "spawn_point") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            std::string currentLevelPath = "";
            if(element->Attribute("previous")) {
                currentLevelPath = element->Attribute("previous");
            }
            if(!firstSpawnPointSet || currentLevelPath == TA::previousLevelPath) {
                spawnPoint = position;
                if(element->Attribute("direction")) {
                    spawnFlip = element->Attribute("direction", "left");
                }
                else {
                    spawnFlip = false;
                }
                firstSpawnPointSet = true;
            }
        }

        else if(name == "level_transition") {
            TA_Point topLeft(element->IntAttribute("left"), element->IntAttribute("top"));
            TA_Point bottomRight(element->IntAttribute("right"), element->IntAttribute("bottom"));
            std::string levelPath = element->Attribute("path");
            spawnObject<TA_Transition>(topLeft, bottomRight, levelPath);
        }

        else if(name == "map_transition") {
            TA_Point topLeft(element->IntAttribute("left"), element->IntAttribute("top"));
            TA_Point bottomRight(element->IntAttribute("right"), element->IntAttribute("bottom"));
            int selection = element->IntAttribute("selection");
            bool seaFox = element->Attribute("seafox", "true");
            spawnObject<TA_Transition>(topLeft, bottomRight, selection, seaFox);
        }

        else if(name == "bridge") {
            std::string path = element->Attribute("path");
            std::string particlePath = element->Attribute("particle_path");
            int left = element->IntAttribute("leftx");
            int right = element->IntAttribute("rightx");
            int y = element->IntAttribute("y");
            for(int x = left; x <= right; x += 16) {
                spawnObject<TA_Bridge>(TA_Point(x, y), path, particlePath);
            }
        }

        else if(name == "camera_lock_point") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            links.camera->setLockPosition(position);
        }

        else if(name == "bird_walker") {
            double floorY = element->IntAttribute("floor_y");
            spawnObject<TA_BirdWalker>(floorY);
        }

        else if(name == "bat_robot") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_BatRobot>(position);
        }

        else if(name == "sound") {
            TA::sound::playMusic(element->Attribute("path"));
        }

        else if(name == "nezu") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_Nezu>(position);
        }

        else if(name == "flame") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            if(element->Attribute("speed")) {
                double speed = element->DoubleAttribute("speed");
                spawnObject<TA_FlameLauncher>(position, speed);
            }
            else {
                spawnObject<TA_FlameLauncher>(position);
            }
        }

        else if(name == "fire") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            bool direction = false;
            if(element->Attribute("direction", "right")) {
                direction = true;
            }
            spawnObject<TA_Fire>(position, direction);
        }

        else if(name == "item_box") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            int number = element->IntAttribute("number");
            std::string itemName = element->Attribute("item_name");
            spawnObject<TA_ItemBox>(position, TA_Point(0, 0), number, itemName);
        }

        else if(name == "drill_mole") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_DrillMole>(position);
        }

        else if(name == "moving_platform") {
            TA_Point startPosition(element->IntAttribute("start_x"), element->IntAttribute("start_y"));
            TA_Point endPosition(element->IntAttribute("end_x"), element->IntAttribute("end_y"));
            if(element->Attribute("idle", "false")) {
                spawnObject<TA_MovingPlatform>(startPosition, endPosition, false);
            }
            else {
                spawnObject<TA_MovingPlatform>(startPosition, endPosition, true);
            }
        }

        else if(name == "bomb_thrower") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            double leftX = element->IntAttribute("left_x"), rightX = element->IntAttribute("right_x");
            spawnObject<TA_BombThrower>(position, leftX, rightX);
        }

        else if(name == "rock_thrower") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            bool direction = element->Attribute("direction", "right");
            spawnObject<TA_RockThrower>(position, direction);
        }

        else if(name == "jumper") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_Jumper>(position);
        }

        else if(name == "wind") {
            TA_Point topLeft(element->IntAttribute("left"), element->IntAttribute("top"));
            TA_Point bottomRight(element->IntAttribute("right"), element->IntAttribute("bottom"));
            TA_Point velocity(element->DoubleAttribute("xsp"), element->DoubleAttribute("ysp"));
            spawnObject<TA_Wind>(topLeft, bottomRight, velocity);
        }
        
        else if(name == "strong_wind") {
            TA_Point topLeft(element->IntAttribute("left"), element->IntAttribute("top"));
            TA_Point bottomRight(element->IntAttribute("right"), element->IntAttribute("bottom"));
            spawnObject<TA_StrongWind>(topLeft, bottomRight);
        }

        else if(name == "speedy") {
            spawnObject<TA_Speedy>();
        }

        else if(name == "mecha_golem") {
            spawnObject<TA_MechaGolem>();
        }

        else if(name == "grass_block") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            std::string path = element->Attribute("path");
            spawnObject<TA_GrassBlock>(position, path);
        }

        else if(name == "mini_sub") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_MiniSub>(position);
        }

        else if(name == "mine") {
            TA_Point position(element->IntAttribute("x"), element->IntAttribute("y"));
            spawnObject<TA_EnemyMine>(position);
        }

        else if(name == "conveyor_belt") {
            TA_Point topLeft(element->IntAttribute("left"), element->IntAttribute("top"));
            TA_Point bottomRight(element->IntAttribute("right"), element->IntAttribute("bottom"));
            bool direction = element->Attribute("direction", "right");
            spawnObject<TA_ConveyorBelt>(topLeft, bottomRight, direction);
        }

        else {
            TA::handleError("Unknown object %s", name.c_str());
        }
    }
}

void TA_ObjectSet::update()
{
    for(TA_Object *currentObject : deleteList) {
        delete currentObject;
    }
    for(TA_Object *currentObject : spawnedObjects) {
        objects.push_back(currentObject);
    }
    deleteList.clear();
    spawnedObjects.clear();

    hitboxContainer.clear();
    for(TA_Object *currentObject : objects) {
        hitboxContainer.add(currentObject->hitbox, currentObject->getCollisionType());
        for(TA_Object::HitboxVectorElement &element : currentObject->hitboxVector) {
            hitboxContainer.add(element.hitbox, element.collisionType);
        }
    }

    std::vector<TA_Object*> newObjects;
    for(TA_Object *currentObject : objects) {
        if(currentObject->update()) {
            newObjects.push_back(currentObject);
        }
        else {
            deleteList.push_back(currentObject);
        }
    }
    objects = newObjects;
}

void TA_ObjectSet::draw(int priority)
{
    for(TA_Object *currentObject : objects) {
        if(currentObject->getDrawPriority() == priority) {
            currentObject->setUpdateAnimation(!isPaused());
            currentObject->draw();
        }
    }
}

void TA_ObjectSet::checkCollision(TA_Polygon &hitbox, int &flags, int halfSolidTop)
{
    if(hitbox.empty()) {
        return;
    }

    flags = links.tilemap->checkCollision(hitbox, halfSolidTop);
    flags |= hitboxContainer.getCollisionFlags(hitbox);

    if(links.character && links.character->getHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_CHARACTER;
    }
    else if(links.seaFox && links.seaFox->getHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_CHARACTER;
    }

    if(links.character && links.character->isUsingHammer() && links.character->getHammerHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_HAMMER;
    }

    if(links.seaFox && links.seaFox->getDrillHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_DRILL;
    }
}

int TA_ObjectSet::checkCollision(TA_Polygon &hitbox)
{
    int flags = 0;
    checkCollision(hitbox, flags);
    return flags;
}

TA_Point TA_ObjectSet::getCharacterPosition()
{
    if(links.character) {
        return links.character->getPosition() + TA_Point(24, 24);
    }
    return links.seaFox->getPosition() + TA_Point(16, 16);
}

void TA_ObjectSet::addRings(int count)
{
    int rings = TA::save::getSaveParameter("rings");
    rings += count;
    rings = std::min(rings, getMaxRings());
    TA::save::setSaveParameter("rings", rings);
}

void TA_ObjectSet::addRingsToMaximum()
{
    TA::save::setSaveParameter("rings", getMaxRings());
}

int TA_ObjectSet::getEmeraldsCount()
{
    long long itemMask = TA::save::getSaveParameter("item_mask");
    int count = 0;
    for(int item = 29; item <= 34; item ++) {
        if(itemMask & (1ll << item)) {
            count ++;
        }
    }
    return count;
}

bool TA_ObjectSet::isVisible(TA_Polygon &hitbox)
{
    TA_Point cameraPosition = links.camera->getPosition();
    TA_Polygon cameraRect;
    cameraRect.setRectangle(cameraPosition - TA_Point(5, 5), cameraPosition + TA_Point(TA::screenWidth + 5, TA::screenHeight + 5));
    return cameraRect.intersects(hitbox);
}

TA_ObjectSet::~TA_ObjectSet()
{
    for(TA_Object *currentObject : objects) {
        delete currentObject;
    }
}
