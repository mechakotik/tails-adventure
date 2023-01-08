#include "sound.h"
#include "error.h"
#include "resource_manager.h"

bool TA::sound::isPlaying(TA_SoundChannel channel)
{
    return Mix_Playing(channel);
}

void TA_Sound::load(std::string filename, TA_SoundChannel newChannel, bool newLoop)
{
    chunk = TA::resmgr::loadChunk(filename);
    channel = newChannel;
    loop = newLoop;
}

void TA_Sound::play()
{
    Mix_PlayChannel(channel, chunk, loop);
}

void TA_Sound::fadeOut(int time)
{
    Mix_FadeOutChannel(channel, time * 1000 / 60);
}
