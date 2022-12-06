#ifndef TA_OBJECT_SET_H
#define TA_OBJECT_SET_H

#include <vector>
#include "geometry.h"
#include "pawn.h"

class TA_ObjectSet;

class TA_Object : public TA_Pawn {
private:
    TA_ObjectSet *objectSet;

public:
    TA_Object(TA_ObjectSet *newObjectSet) {objectSet = newObjectSet;}
    virtual bool update() {return false;}
};

class TA_ObjectSet {
private:
    std::vector<TA_Object*> objects;
    void spawnObject(TA_Object *object);

public:
    void update();
    void draw();
    void spawnExplosion(TA_Point position);
};

#endif // TA_OBJECT_SET_H
