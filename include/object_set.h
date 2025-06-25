#ifndef TA_OBJECT_SET_H
#define TA_OBJECT_SET_H

#include <toml.hpp>
#include <vector>
#include "character.h"
#include "geometry.h"
#include "hitbox_container.h"
#include "links.h"
#include "screen.h"
#include "tilemap.h"
#include "tools.h"

enum TA_COLLISION_STATE {
    TA_GROUND_COLLISION = (1 << 0),
    TA_WALL_COLLISION = (1 << 1),
    TA_CEIL_COLLISION = (1 << 2),
    TA_COLLISION_ERROR = (1 << 3)
};

class TA_ObjectSet;
enum TA_BombMode : int;

class TA_Object : public TA_Sprite {
    friend class TA_ObjectSet;

protected:
    virtual void updatePosition();

    TA_ObjectSet* objectSet;
    TA_Point position;
    TA_Rect hitbox;
    int collisionType = TA_COLLISION_TRANSPARENT;

    struct HitboxVectorElement {
        TA_Rect hitbox;
        int collisionType;
    };
    std::vector<HitboxVectorElement> hitboxVector;

public:
    TA_Object(TA_ObjectSet* newObjectSet);
    virtual bool update() { return false; }
    virtual bool checkCollision(TA_Rect rv) {
        return collisionType != TA_COLLISION_TRANSPARENT && hitbox.intersects(rv);
    }
    virtual int getDrawPriority() { return 0; }
    TA_Point getDistanceToCharacter();
    virtual void destroy() {}
    virtual ~TA_Object() = default;
};

class TA_ObjectSet {
private:
    void tryLoad(std::string filename);
    void loadObject(std::string name, toml::value object);

    std::vector<TA_Object*> objects, spawnedObjects, deleteList;
    TA_Links links;
    TA_HitboxContainer hitboxContainer;
    TA_Point spawnPoint;
    TA_ScreenState transition = TA_SCREENSTATE_CURRENT;
    bool spawnFlip = false, firstSpawnPointSet = false;
    bool paused = false;
    bool night = false;
    float waterLevel = -64;

    // moveAndCollide helpers
    void moveByX();
    void moveByY();
    void popOut(float area);
    float getFirstGood(TA_Point add);
    bool isGoodPosition(TA_Point position);

    TA_Point position, topLeft, bottomRight, velocity, delta;
    int solidFlags;
    bool ground;

public:
    ~TA_ObjectSet();
    TA_Point getCharacterPosition();
    TA_Point getCharacterSpawnPoint() { return spawnPoint; }
    bool getCharacterSpawnFlip() { return spawnFlip; }

    void setLinks(TA_Links newLinks) { links = newLinks; }
    TA_Links getLinks() { return links; }

    void load(std::string filename);
    void update();
    void draw(int priority);

    void checkCollision(TA_Rect& hitbox, int& flags);
    int checkCollision(TA_Rect& hitbox);
    std::pair<TA_Point, int> moveAndCollide(TA_Point position, TA_Point topLeft, TA_Point bottomRight,
        TA_Point velocity, int solidFlags, bool ground = false);
    int getCollisionFlags(TA_Point position, TA_Point topLeft, TA_Point bottomRight, int solidFlags);
    void setTransition(TA_ScreenState screenState) { transition = screenState; }
    TA_ScreenState getTransition() { return transition; }
    bool hasCollisionType(TA_CollisionType type) { return hitboxContainer.hasCollisionType(type); }
    bool enemyShouldDropRing() { return TA::random::next() % 4 == 0; }
    void resetInstaShield() {
        if(links.character) links.character->resetInstaShield();
    } // TODO: figure out what it is
    bool isPaused() { return paused; }
    void setPaused(bool enabled) { paused = enabled; }
    bool isVisible(TA_Rect& hitbox);

    int getEmeraldsCount();
    int getMaxRings() { return 8 + 2 * getEmeraldsCount(); }
    void addRings(int count);
    void addRingsToMaximum();
    bool isNight() { return night; }
    void disableNight() { night = false; }
    float getWaterLevel() { return waterLevel; }

    template <class T, typename... P>
    void spawnObject(P... params) {
        auto* object = new T(this);
        object->load(params...);
        spawnedObjects.push_back(object);
    }
};

#endif // TA_OBJECT_SET_H
