#include "hitbox_container.h"

void TA_HitboxContainer::add(TA_Polygon &hitbox, TA_CollisionType type)
{
    Element element = {&hitbox, type};

    auto addToChunk = [&](Chunk &chunk) {
        lazyClear(chunk);
        chunk.elements.push_back(element);
    };

    TA_Point topLeft = hitbox.getTopLeft(), bottomRight = hitbox.getBottomRight();
    int left = topLeft.x / chunkSize, top = topLeft.y / chunkSize, right = bottomRight.x / chunkSize, bottom = bottomRight.y / chunkSize;

    if(right - left <= 1 && bottom - top <= 1) {
        addToChunk(chunks[top][left]);
        if(right != left) {
            addToChunk(chunks[top][right]);
        }
        if(bottom != top) {
            addToChunk(chunks[bottom][left]);
        }
        if(right != left && bottom != top) {
            addToChunk(chunks[bottom][right]);
        }
    }
    else {
        addToChunk(commonChunk);
    }
}

int TA_HitboxContainer::getCollisionFlags(TA_Polygon &hitbox)
{
    int flags = 0;

    auto processChunk = [&](Chunk &chunk) {
        lazyClear(chunk);
        for(Element &element : chunk.elements) {
            if(hitbox.intersects(*element.hitbox)) {
                flags |= element.type;
            }
        }
    };

    TA_Point topLeft = hitbox.getTopLeft(), bottomRight = hitbox.getBottomRight();
    int left = topLeft.x / chunkSize, top = topLeft.y / chunkSize, right = bottomRight.x / chunkSize, bottom = bottomRight.y / chunkSize;

    processChunk(commonChunk);
    processChunk(chunks[top][left]);
    if(right != left) {
        processChunk(chunks[top][right]);
    }
    if(bottom != top) {
        processChunk(chunks[bottom][left]);
    }
    if(right != left && bottom != top) {
        processChunk(chunks[bottom][right]);
    }

    return flags;
}

void TA_HitboxContainer::lazyClear(Chunk &chunk)
{
    if(chunk.updateTime == currentTime) {
        return;
    }
    chunk.elements.clear();
    chunk.updateTime = currentTime;
}
