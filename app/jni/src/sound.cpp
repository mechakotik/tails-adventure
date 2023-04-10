#include "sound.h"
#include "error.h"
#include "resource_manager.h"

bool TA::sound::isPlaying(TA_SoundChannel channel)
{
    return Mix_Playing(channel);
}

void TA::sound::fadeOut(int time)
{
    for(int channel = 0; channel < TA_SOUND_CHANNEL_MAX; channel ++) {
        Mix_FadeOutChannel(channel, time * 1000 / 60);
    }
}

void TA_Sound::load(std::string filename, TA_SoundChannel newChannel, bool newLoop)
{
    chunk = TA::resmgr::loadChunk(filename);
    channel = newChannel;
    loop = newLoop;
}

void TA_Sound::play()
{
    if(channel == TA_SOUND_CHANNEL_MUSIC) {
        Mix_Volume(channel, MIX_MAX_VOLUME);
    }
    Mix_PlayChannel(channel, chunk, loop);
}

void TA_Sound::fadeOut(int time)
{
    Mix_FadeOutChannel(channel, time * 1000 / 60);
}
