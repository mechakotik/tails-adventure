#ifndef TA_OBJECT_SET_H
#define TA_OBJECT_SET_H

#include <vector>
#include "engine/geometry.h"
#include "engine/pawn.h"
#include "engine/camera.h"

class TA_ObjectSet;

class TA_Object : public TA_Pawn {
private:
    TA_ObjectSet *objectSet;

public:
    TA_Object(TA_ObjectSet *newObjectSet);
    virtual bool update() {return false;}
};

class TA_ObjectSet {
private:
    std::vector<TA_Object*> objects;
    TA_Camera *camera;

    void spawnObject(TA_Object *object);

public:
    void setCamera(TA_Camera *newCamera) {camera = newCamera;}
    TA_Camera *getCamera() {return camera;}
    void update();
    void draw();
    void spawnExplosion(TA_Point position);
};

#endif // TA_OBJECT_SET_H
