#ifndef TA_HITBOX_CONTAINER_H
#define TA_HITBOX_CONTAINER_H

#include <array>
#include <vector>
#include "defs.h"
#include "geometry.h"

class TA_HitboxContainer {
private:
    static const int size = 1e4, chunkSize = 128;
    static const int sizeChunks = (size + chunkSize - 1) / chunkSize;

    struct Element {
        TA_Rect hitbox;
        int type;
    };

    struct Chunk {
        std::vector<int> elements;
        int updateTime = 0;
    };

    std::array<std::array<Chunk, sizeChunks>, sizeChunks> chunks;
    std::vector<Element> hitboxes;
    Chunk commonChunk;
    int currentTime = 0, collisionTypeMask = 0;

    void lazyClear(Chunk& chunk);

public:
    void add(const TA_Rect& hitbox, int type);
    int getCollisionFlags(const TA_Rect& hitbox);
    bool hasCollisionType(TA_CollisionType type) { return collisionTypeMask & type; }
    void clear();
};

#endif // TA_HITBOX_CONTAINER_H
