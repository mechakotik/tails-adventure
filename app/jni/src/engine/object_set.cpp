#include "object_set.h"
#include "objects/explosion.h"
#include "engine/error.h"

TA_Object::TA_Object(TA_ObjectSet *newObjectSet)
{
    objectSet = newObjectSet;
    setCamera(objectSet->getCamera());
}

void TA_ObjectSet::update()
{
    std::vector<TA_Object*> newObjects;
    for(TA_Object *currentObject : objects) {
        if(currentObject->update()) {
            newObjects.push_back(currentObject);
        }
        else {
            delete currentObject;
        }
    }
    objects = newObjects;
}

void TA_ObjectSet::draw()
{
    for(TA_Object *currentObject : objects) {
        currentObject->draw();
    }
}

void TA_ObjectSet::spawnObject(TA_Object *object)
{
    objects.push_back(object);
}

void TA_ObjectSet::spawnExplosion(TA_Point position)
{
    auto *explosion = new TA_Explosion(this);
    explosion->load(position);
    spawnObject(explosion);
}
