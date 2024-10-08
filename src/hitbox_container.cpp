#include "hitbox_container.h"

void TA_HitboxContainer::add(TA_Polygon &hitbox, int type)
{
    if(type == TA_COLLISION_TRANSPARENT) {
        return;
    }
    collisionTypeMask |= type;

    if(hitbox.empty()) {
        return;
    }
    Element element = {&hitbox, type};

    auto addToChunk = [&](Chunk &chunk) {
        lazyClear(chunk);
        chunk.elements.push_back(element);
    };

    TA_Point topLeft = hitbox.getTopLeft(), bottomRight = hitbox.getBottomRight();
    int left = topLeft.x / chunkSize, top = topLeft.y / chunkSize, right = bottomRight.x / chunkSize, bottom = bottomRight.y / chunkSize;

    if(0 <= top && bottom <= size && 0 <= left && right < size && right - left <= 1 && bottom - top <= 1) {
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

    auto processChunkAt = [&](int x, int y) {
        if(0 <= x && x < (int)chunks.size() && 0 <= y && y <= (int)chunks[x].size()) {
            processChunk(chunks[x][y]);
        }
    };

    TA_Point topLeft = hitbox.getTopLeft(), bottomRight = hitbox.getBottomRight();
    int left = topLeft.x / chunkSize, top = topLeft.y / chunkSize, right = bottomRight.x / chunkSize, bottom = bottomRight.y / chunkSize;

    processChunk(commonChunk);
    processChunkAt(top, left);
    if(right != left) {
        processChunkAt(top, right);
    }
    if(bottom != top) {
        processChunkAt(bottom, left);
    }
    if(right != left && bottom != top) {
        processChunkAt(bottom, right);
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

void TA_HitboxContainer::clear()
{
    currentTime ++;
    collisionTypeMask = 0;
}
