#ifndef TA_OBJECT_SET_H
#define TA_OBJECT_SET_H

#include <vector>
#include "engine/geometry.h"
#include "engine/pawn.h"
#include "engine/camera.h"
#include "engine/tilemap.h"

class TA_ObjectSet;

class TA_Object : public TA_Pawn {
private:
    virtual bool checkPawnCollision(TA_Polygon rv) override {return hitbox.intersects(rv);}

public:
    TA_ObjectSet *objectSet;
    TA_Polygon hitbox;

    TA_Object(TA_ObjectSet *newObjectSet);
    virtual bool update() {return false;}
    virtual bool checkCollision(TA_Polygon rv) {return hitbox.intersects(rv);}
    virtual TA_CollisionType getCollisionType() {return TA_COLLISION_TRANSPARENT;}
    virtual void destroy() {}
};

class TA_ObjectSet {
private:
    std::vector<TA_Object*> objects, spawnedObjects;
    TA_Camera *camera;
    TA_Tilemap *tilemap;

    void spawnObject(TA_Object *object);

public:
    void setCamera(TA_Camera *newCamera) {camera = newCamera;}
    TA_Camera *getCamera() {return camera;}
    void setCollisionTilemap(TA_Tilemap *newTilemap) {tilemap = newTilemap;}

    void update();
    void draw();
    void spawnExplosion(TA_Point position);
    void spawnBomb(TA_Point position, bool direction);
    void checkCollision(TA_Polygon hitbox, int &flags);
};

#endif // TA_OBJECT_SET_H
