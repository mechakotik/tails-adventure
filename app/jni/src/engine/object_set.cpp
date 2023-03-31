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
#include "engine/error.h"
#include "engine/tools.h"
#include "tinyxml2.h"

TA_Object::TA_Object(TA_ObjectSet *newObjectSet)
{
    objectSet = newObjectSet;
    setCamera(objectSet->getCamera());
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
            spawnObject<TA_BreakableBlock>(position, dropsRing);
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
            spawnPoint = position;
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
        if(currentObject->getCollisionType() != TA_COLLISION_TRANSPARENT) {
            hitboxContainer.add(currentObject->hitbox, currentObject->getCollisionType());
        }
        for(TA_Object::HitboxVectorElement &element : currentObject->hitboxVector) {
            if(element.collisionType != TA_COLLISION_TRANSPARENT) {
                hitboxContainer.add(element.hitbox, element.collisionType);
            }
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

void TA_ObjectSet::checkCollision(TA_Polygon &hitbox, int &flags)
{
    flags = 0;
    tilemap->checkCollision(hitbox, flags);
    flags |= hitboxContainer.getCollisionFlags(hitbox);
    /*for(TA_Object *currentObject : deleteList) {
        if(currentObject->checkCollision(hitbox)) {
            flags |= currentObject->getCollisionType();
        }
    }*/
    if(characterHitbox->intersects(hitbox)) {
        flags |= TA_COLLISION_CHARACTER;
    }
}

int TA_ObjectSet::checkCollision(TA_Polygon &hitbox)
{
    int flags = 0;
    checkCollision(hitbox, flags);
    return flags;
}

TA_ObjectSet::~TA_ObjectSet()
{
    for(TA_Object *currentObject : objects) {
        delete currentObject;
    }
}
