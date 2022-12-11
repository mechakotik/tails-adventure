#include "engine/game.h"

int main(int argc, char* args[])
{
    TA_Game game;

    while(game.process()) {
        game.update();
    }

    return 0;
}
