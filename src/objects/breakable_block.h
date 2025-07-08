#ifndef TA_BREAKABLE_BLOCK_H
#define TA_BREAKABLE_BLOCK_H

#include "object_set.h"

class TA_BreakableBlock : public TA_Object {
private:
    std::string particlePath;
    const float grv = 0.35;
    bool dropsRing;
    bool strong;
    TA_Sound breakSound;

public:
    using TA_Object::TA_Object;
    void load(std::string path, std::string particlePath, TA_Point position, bool dropsRing, bool strong);
    bool update() override;
    int getDrawPriority() override { return 0; }
};

#endif // TA_BREAKABLE_BLOCK_H
