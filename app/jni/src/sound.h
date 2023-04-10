#ifndef TA_SOUND_H
#define TA_SOUND_H

#include <string>
#include "SDL_mixer.h"

enum TA_SoundChannel
{
    TA_SOUND_CHANNEL_MUSIC,
    TA_SOUND_CHANNEL_SFX,
    TA_SOUND_CHANNEL_MAX
};

namespace TA { namespace sound {
    bool isPlaying(TA_SoundChannel channel);
    void fadeOut(int time);
}}

class TA_Sound
{
private:
    Mix_Chunk *chunk;
    TA_SoundChannel channel;
    bool loop;

public:
    void load(std::string filename, TA_SoundChannel channel, bool loop = false);
    void play();
    void fadeOut(int time);
};

#endif // TA_SOUND_H
