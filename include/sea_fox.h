#ifndef TA_SEA_FOX_H
#define TA_SEA_FOX_H

#include "pawn.h"
#include "links.h"

class TA_SeaFox : public TA_Pawn {
private:
    TA_Links links;
    
public:
    void load(TA_Links links);
    void update();
};

#endif // TA_SEA_FOX_H
