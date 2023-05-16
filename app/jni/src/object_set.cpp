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
#include "tools.h"
#include "tinyxml2.h"
#include "objects/transition.h"
#include "objects/bridge.h"
#include "objects/bird_walker.h"
#include "character.h"

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

void TA_ObjectSet::load(std::string filename)
{
    tinyxml2::XMLDocument file;
    file.Parse(TA::readStringFromFile(filename).c_str());

    for(tinyxml2::XMLElement *element = file.FirstChildElement("objects")->FirstChildElement("object");
        element != nullptr; element = element->NextSiblingElement("object"))
    {
        std::string name = element->Attribute("name");

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
                    spawnFlip = (strcmp(element->Attribute("direction"), "left") == 0);
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
            spawnObject<TA_Transition>(topLeft, bottomRight, TA_SCREENSTATE_GAME, levelPath);
        }

        else if(name == "pf_bridge") {
            int left = element->IntAttribute("leftx");
            int right = element->IntAttribute("rightx");
            int y = element->IntAttribute("y");
            for(int x = left; x <= right; x += 16) {
                spawnObject<TA_Bridge>(TA_Point(x, y), "maps/pf/pf_bridge.png", "maps/pf/pf_bridge_part.png");
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

        else {
            TA::printWarning("Unknown object %s", name.c_str());
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
            currentObject->draw();
        }
    }
}

void TA_ObjectSet::checkCollision(TA_Polygon &hitbox, int &flags, int halfSolidTop)
{
    if(hitbox.empty()) {
        return;
    }
    flags = 0;
    links.tilemap->checkCollision(hitbox, flags, halfSolidTop);
    flags |= hitboxContainer.getCollisionFlags(hitbox);
    /*for(TA_Object *currentObject : deleteList) {
        if(currentObject->checkCollision(hitbox)) {
            flags |= currentObject->getCollisionType();
        }
    }*/
    if(links.character->getHitbox()->intersects(hitbox)) {
        flags |= TA_COLLISION_CHARACTER;
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
    return links.character->getPosition();
}

TA_ObjectSet::~TA_ObjectSet()
{
    for(TA_Object *currentObject : objects) {
        delete currentObject;
    }
}
