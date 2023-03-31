#ifndef TA_TRANSITION_H
#define TA_TRANSITION_H

#include "engine/object_set.h"
#include "engine/screen.h"

class TA_Transition : public TA_Object {
private:
    TA_ScreenState screenState;
    std::string levelPath;

public:
    using TA_Object::TA_Object;
    void load(TA_Point topLeft, TA_Point bottomRight, TA_ScreenState screenState, std::string levelPath = "");
    bool update() override;
    void draw() override;
};

#endif // TA_TRANSITION_H
