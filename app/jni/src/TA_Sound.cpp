#include "TA_Sound.h"
#include "TA_Error.h"

void TA_Sound::load(std::string filename, TA_SoundChannel newChannel, bool newLoop)
{
    chunk = Mix_LoadWAV(filename.c_str());
    if(chunk == nullptr) {
        handleSDLError("%s load failed", filename.c_str());
    }
    channel = newChannel;
    loop = newLoop;
}

void TA_Sound::play()
{
    Mix_PlayChannel(channel, chunk, loop);
}
