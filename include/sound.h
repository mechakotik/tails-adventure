#ifndef TA_SOUND_H
#define TA_SOUND_H

#include <string>
#include "SDL_mixer.h"

enum TA_SoundChannel
{
    TA_SOUND_CHANNEL_MUSIC,
    TA_SOUND_CHANNEL_SFX1,
    TA_SOUND_CHANNEL_SFX2,
    TA_SOUND_CHANNEL_SFX3,
    TA_SOUND_CHANNEL_MAX
};

namespace TA { namespace sound {
    void update();
    bool isPlaying(TA_SoundChannel channel);
    void fadeOut(int time);
    void fadeOutChannel(TA_SoundChannel channel, int time);
}}

class TA_Sound
{
private:
    Mix_Chunk *chunk = nullptr;
    TA_SoundChannel channel;
    bool loop;

public:
    void load(std::string filename, TA_SoundChannel channel, bool loop = false);
    void play();
    void fadeOut(int time);
};

#endif // TA_SOUND_H
