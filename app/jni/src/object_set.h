#ifndef TA_OBJECT_SET_H
#define TA_OBJECT_SET_H

#include <vector>
#include "geometry.h"
#include "pawn.h"
#include "camera.h"
#include "tilemap.h"
#include "hitbox_container.h"
#include "screen.h"

class TA_ObjectSet;
enum TA_BombMode : int;

class TA_Object : public TA_Pawn {
private:
    virtual bool checkPawnCollision(TA_Polygon &rv) override {return false;}

protected:
    virtual void updatePosition();

public:
    TA_ObjectSet *objectSet;
    TA_Polygon hitbox;

    struct HitboxVectorElement {
        TA_Polygon hitbox;
        TA_CollisionType collisionType;
    };
    std::vector<HitboxVectorElement> hitboxVector;

    TA_Object(TA_ObjectSet *newObjectSet);
    virtual bool update() {return false;}
    virtual bool checkCollision(TA_Polygon rv) {return getCollisionType() != TA_COLLISION_TRANSPARENT && hitbox.intersects(rv);}
    virtual TA_CollisionType getCollisionType() {return TA_COLLISION_TRANSPARENT;}
    virtual int getDrawPriority() {return 0;}
    virtual void destroy() {}
};

class TA_ObjectSet {
private:
    std::vector<TA_Object*> objects, spawnedObjects, deleteList;
    TA_Camera *camera;
    TA_Tilemap *tilemap;
    TA_Polygon *characterHitbox;
    TA_HitboxContainer hitboxContainer;
    TA_Point spawnPoint;
    TA_ScreenState transition = TA_SCREENSTATE_CURRENT;
    bool spawnFlip = false, firstSpawnPointSet = false;

public:
    ~TA_ObjectSet();
    void setCamera(TA_Camera *newCamera) {camera = newCamera;}
    TA_Camera* getCamera() {return camera;}
    void setCollisionTilemap(TA_Tilemap *newTilemap) {tilemap = newTilemap;}
    void setCharacterHitbox(TA_Polygon *newHitbox) {characterHitbox = newHitbox;}
    TA_Point getCharacterPosition() {return characterHitbox->getVertex(0);}
    TA_Point getCharacterSpawnPoint() {return spawnPoint;}
    bool getCharacterSpawnFlip() {return spawnFlip;}

    void load(std::string filename);
    void update();
    void draw(int priority);
    void checkCollision(TA_Polygon &hitbox, int &flags, int halfSolidTop = -1e9);
    int checkCollision(TA_Polygon &hitbox);
    void setTransition(TA_ScreenState screenState) {transition = screenState;}
    TA_ScreenState getTransition() {return transition;}
    bool hasCollisionType(TA_CollisionType type) {return hitboxContainer.hasCollisionType(type);}

    template<class T, typename... P>
    void spawnObject(P... params) {
        auto *object = new T(this);
        object->load(params...);
        spawnedObjects.push_back(object);
    }
};

#endif // TA_OBJECT_SET_H
