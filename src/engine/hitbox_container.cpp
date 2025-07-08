#include "hitbox_container.h"

void TA_HitboxContainer::add(const TA_Rect& hitbox, int type) {
    if(type == TA_COLLISION_TRANSPARENT) {
        return;
    }
    collisionTypeMask |= type;

    Element element = {.hitbox = hitbox, .type = type};
    hitboxes.push_back(element);
    int id = static_cast<int>(hitboxes.size()) - 1;

    auto addToChunk = [&](Chunk& chunk) {
        lazyClear(chunk);
        chunk.elements.push_back(id);
    };

    TA_Point topLeft = hitbox.getTopLeft();
    TA_Point bottomRight = hitbox.getBottomRight();
    int left = static_cast<int>(topLeft.x / chunkSize);
    int top = static_cast<int>(topLeft.y / chunkSize);
    int right = static_cast<int>(bottomRight.x / chunkSize);
    int bottom = static_cast<int>(bottomRight.y / chunkSize);

    if(0 <= top && bottom < size && 0 <= left && right < size && right - left <= 1 && bottom - top <= 1) {
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
    } else {
        addToChunk(commonChunk);
    }
}

int TA_HitboxContainer::getCollisionFlags(const TA_Rect& hitbox) {
    int flags = 0;

    auto processChunk = [&](Chunk& chunk) {
        lazyClear(chunk);
        for(int id : chunk.elements) {
            if(hitbox.intersects(hitboxes[id].hitbox)) {
                flags |= hitboxes[id].type;
            }
        }
    };

    auto processChunkAt = [&](int x, int y) {
        if(0 <= x && x < (int)chunks.size() && 0 <= y && y <= (int)chunks[x].size()) {
            processChunk(chunks[x][y]);
        }
    };

    TA_Point topLeft = hitbox.getTopLeft(), bottomRight = hitbox.getBottomRight();
    int left = topLeft.x / chunkSize, top = topLeft.y / chunkSize, right = bottomRight.x / chunkSize,
        bottom = bottomRight.y / chunkSize;

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

void TA_HitboxContainer::lazyClear(Chunk& chunk) {
    if(chunk.updateTime == currentTime) {
        return;
    }
    chunk.elements.clear();
    chunk.updateTime = currentTime;
}

void TA_HitboxContainer::clear() {
    hitboxes.clear();
    currentTime++;
    collisionTypeMask = 0;
}
