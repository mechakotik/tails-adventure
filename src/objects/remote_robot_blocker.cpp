#include "remote_robot_blocker.h"

void TA_RemoteRobotBlocker::load(TA_Point position) {
    loadFromToml("objects/remote_robot_blocker.toml");
    setAnimation("idle");
    this->position = position;
    updatePosition();
}

bool TA_RemoteRobotBlocker::update() {
    if(getCurrentFrame() == 0) {
        collisionType = TA_COLLISION_TRANSPARENT;
    } else {
        collisionType = TA_COLLISION_REMOTE_ROBOT_BLOCKER;
        switch(getCurrentFrame()) {
            case 1:
                hitbox.setRectangle({8, 3}, {14, 6});
                break;
            case 2:
                hitbox.setRectangle({5, 5}, {17, 10});
                break;
            case 3:
                hitbox.setRectangle({0, 9}, {22, 16});
                break;
            default:
                break;
        }
    }

    return true;
}
