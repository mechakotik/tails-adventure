#include "game.h"
#include "tools.h"

int main(int argc, char* argv[])
{
    for(int pos = 1; pos < argc; pos ++) {
        TA::arguments.insert(argv[pos]);
    }

    TA_Game game;

    while(game.process()) {
        game.update();
    }

    return 0;
}
