#include "object_set.h"

class TA_Sparkle : public TA_Object {
public:
    using TA_Object::TA_Object;
    void load(TA_Point position);
    bool update() override;
    int getDrawPriority() override { return 1; }
};
