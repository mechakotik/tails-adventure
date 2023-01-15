#include "object_set.h"
#include "objects/explosion.h"
#include "objects/bomb.h"
#include "objects/breakable_block.h"
#include "engine/error.h"

TA_Object::TA_Object(TA_ObjectSet *newObjectSet)
{
    objectSet = newObjectSet;
    setCamera(objectSet->getCamera());
}

void TA_ObjectSet::update()
{
    std::vector<TA_Object*> newObjects, deleteList;
    for(TA_Object *currentObject : objects) {
        if(currentObject->update()) {
            newObjects.push_back(currentObject);
        }
        else {
            deleteList.push_back(currentObject);
        }
    }
    for(TA_Object *currentObject : deleteList) {
        delete currentObject;
    }
    for(TA_Object *currentObject : spawnedObjects) {
        newObjects.push_back(currentObject);
    }
    spawnedObjects.clear();
    objects = newObjects;
}

void TA_ObjectSet::draw()
{
    for(TA_Object *currentObject : objects) {
        currentObject->draw();
    }
}

void TA_ObjectSet::checkCollision(TA_Polygon hitbox, int &flags)
{
    flags = 0;
    tilemap->checkCollision(hitbox, flags);
    for(TA_Object *currentObject : objects) {
        if(currentObject->checkCollision(hitbox)) {
            flags |= currentObject->getCollisionType();
        }
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

void TA_ObjectSet::spawnBomb(TA_Point position, bool direction)
{
    auto *bomb = new TA_Bomb(this);
    bomb->load(position, direction);
    spawnObject(bomb);
}

void TA_ObjectSet::spawnBreakableBlock(TA_Point position)
{
    auto *block = new TA_BreakableBlock(this);
    block->load(position);
    spawnObject(block);
}

TA_ObjectSet::~TA_ObjectSet()
{
    for(TA_Object *currentObject : objects) {
        delete currentObject;
    }
}
