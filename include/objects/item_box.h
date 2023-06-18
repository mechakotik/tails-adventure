#include "object_set.h"

class TA_ItemBox : public TA_Object {
private:
    int itemNumber = 0;

public:
    using TA_Object::TA_Object;
    void load(TA_Point position, int itemNumber);
    bool update();
};
