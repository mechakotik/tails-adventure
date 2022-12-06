#include "object_set.h"
#include "explosion.h"

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
    TA_Explosion explosion(this);
    explosion.load(position);
    spawnObject(&explosion);
}
