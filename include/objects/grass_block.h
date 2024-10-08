#ifndef TA_GRASS_BLOCK_H
#define TA_GRASS_BLOCK_H

#include "object_set.h"

class TA_GrassBlock : public TA_Object {
private:
    TA_Sound breakSound;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, std::string texture);
    bool update() override;
    int getCollisionType() override {return TA_COLLISION_SOLID;}
};

#endif // TA_GRASS_BLOCK_H
