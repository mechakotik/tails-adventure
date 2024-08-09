#include "sea_fox.h"

void TA_SeaFox::load(TA_Links links)
{
    this->links = links;
    links.camera->setFollowPosition(&position);
}

void TA_SeaFox::update()
{

}
