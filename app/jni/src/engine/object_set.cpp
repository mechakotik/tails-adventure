#include "object_set.h"
#include "objects/explosion.h"
#include "objects/bomb.h"
#include "objects/breakable_block.h"
#include "objects/particle.h"
#include "objects/ring.h"
#include "objects/walker.h"
#include "objects/hover_pod.h"
#include "objects/dead_kukku.h"
#include "engine/error.h"
#include "engine/tools.h"
#include "tinyxml2.h"

TA_Object::TA_Object(TA_ObjectSet *newObjectSet)
{
    objectSet = newObjectSet;
    setCamera(objectSet->getCamera());
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
            spawnBreakableBlock(position, dropsRing);
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
                spawnWalker(position, range, direction);
            }
            else {
                spawnHoverPod(position, range, direction);
            }
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

void TA_ObjectSet::spawnObject(TA_Object *object)
{
    spawnedObjects.push_back(object);
}

void TA_ObjectSet::spawnExplosion(TA_Point position, int delay)
{
    auto *explosion = new TA_Explosion(this);
    explosion->load(position, delay);
    spawnObject(explosion);
}

void TA_ObjectSet::spawnBomb(TA_Point position, bool direction, TA_BombMode mode)
{
    auto *bomb = new TA_Bomb(this);
    bomb->load(position, direction, mode);
    spawnObject(bomb);
}

void TA_ObjectSet::spawnBreakableBlock(TA_Point position, bool dropsRing)
{
    auto *block = new TA_BreakableBlock(this);
    block->load(position, dropsRing);
    spawnObject(block);
}

void TA_ObjectSet::spawnParticle(std::string filename, TA_Point position, TA_Point velocity, TA_Point delta)
{
    auto *particle = new TA_Particle(this);
    particle->load(filename, position, velocity, delta);
    spawnObject(particle);
}

void TA_ObjectSet::spawnRing(TA_Point position)
{
    auto *ring = new TA_Ring(this);
    ring->load(position);
    spawnObject(ring);
}

void TA_ObjectSet::spawnWalker(TA_Point position, int range, bool flip)
{
    auto *walker = new TA_Walker(this);
    walker->load(position, range, flip);
    spawnObject(walker);
}

void TA_ObjectSet::spawnWalkerBullet(TA_Point position, bool direction)
{
    auto *walkerBullet = new TA_WalkerBullet(this);
    walkerBullet->load(position, direction);
    spawnObject(walkerBullet);
}

void TA_ObjectSet::spawnHoverPod(TA_Point position, int range, bool flip)
{
    auto *hoverPod = new TA_HoverPod(this);
    hoverPod->load(position, range, flip);
    spawnObject(hoverPod);
}

void TA_ObjectSet::spawnDeadKukku(TA_Point position)
{
    auto *deadKukku = new TA_DeadKukku(this);
    deadKukku->load(position);
    spawnObject(deadKukku);
}

TA_ObjectSet::~TA_ObjectSet()
{
    for(TA_Object *currentObject : objects) {
        delete currentObject;
    }
}
