#ifndef TA_HITBOX_CONTAINER_H
#define TA_HITBOX_CONTAINER_H

#include <vector>
#include "geometry.h"
#include "tilemap.h"

class TA_HitboxContainer {
private:
    static const int size = 1e4, chunkSize = 128;
    static const int sizeChunks = (size + chunkSize - 1) / chunkSize;

    struct Element {
        TA_Polygon *hitbox;
        int type;
    };

    struct Chunk {
        std::vector<Element> elements;
        int updateTime = 0;
    };

    std::array<std::array<Chunk, sizeChunks>, sizeChunks> chunks;
    Chunk commonChunk;
    int currentTime = 0;

    void lazyClear(Chunk &chunk);

public:
    void add(TA_Polygon &hitbox, TA_CollisionType type);
    int getCollisionFlags(TA_Polygon &hitbox);
    void clear() {currentTime ++;}
};

#endif // TA_HITBOX_CONTAINER_H
