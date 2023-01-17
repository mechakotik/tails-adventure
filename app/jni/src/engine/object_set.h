#ifndef TA_OBJECT_SET_H
#define TA_OBJECT_SET_H

#include <vector>
#include "engine/geometry.h"
#include "engine/pawn.h"
#include "engine/camera.h"
#include "engine/tilemap.h"

class TA_ObjectSet;
enum TA_BombMode : int;

class TA_Object : public TA_Pawn {
private:
    virtual bool checkPawnCollision(TA_Polygon &rv) override {return false;}

public:
    TA_ObjectSet *objectSet;
    TA_Polygon hitbox;

    TA_Object(TA_ObjectSet *newObjectSet);
    virtual bool update() {return false;}
    virtual bool checkCollision(TA_Polygon rv) {return getCollisionType() != TA_COLLISION_TRANSPARENT && hitbox.intersects(rv);}
    virtual TA_CollisionType getCollisionType() {return TA_COLLISION_TRANSPARENT;}
    virtual int getDrawPriority() {return 0;}
    virtual void destroy() {}
};

class TA_ObjectSet {
private:
    std::vector<TA_Object*> objects, spawnedObjects;
    TA_Camera *camera;
    TA_Tilemap *tilemap;

    void spawnObject(TA_Object *object);

public:
    ~TA_ObjectSet();
    void setCamera(TA_Camera *newCamera) {camera = newCamera;}
    TA_Camera *getCamera() {return camera;}
    void setCollisionTilemap(TA_Tilemap *newTilemap) {tilemap = newTilemap;}

    void update();
    void draw(int priority);
    void checkCollision(TA_Polygon &hitbox, int &flags);

    void spawnExplosion(TA_Point position, int delay = 0);
    void spawnBomb(TA_Point position, bool direction, TA_BombMode mode);
    void spawnBreakableBlock(TA_Point position, bool dropsRing = false);
    void spawnParticle(std::string filename, TA_Point position, TA_Point velocity, TA_Point delta);
    void spawnRing(TA_Point position);
};

#endif // TA_OBJECT_SET_H
