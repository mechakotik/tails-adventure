#ifndef TA_ENEMY_BOMB_H
#define TA_ENEMY_BOMB_H

#include "object_set.h"

class TA_EnemyBomb : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(const std::filesystem::path& path, TA_Point position, TA_Point topLeft, TA_Point bottomRight,
        TA_Point velocity);
    bool update() override;

private:
    static constexpr float grv = 0.125;

    TA_Point topLeft;
    TA_Point bottomRight;
    TA_Point velocity;
};

#endif
