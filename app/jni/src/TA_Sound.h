#ifndef TA_SOUND_H
#define TA_SOUND_H

#include <string>
#include <SDL_mixer.h>

enum TA_SoundChannel
{
    TA_SOUND_CHANNEL_MUSIC,
    TA_SOUND_CHANNEL_SFX,
    TA_SOUND_CHANNEL_MAX
};

class TA_Sound
{
private:
    Mix_Chunk *chunk;
    TA_SoundChannel channel;
    bool loop;

public:
    void load(std::string filename, TA_SoundChannel channel, bool loop);
    void play();
};

#endif // TA_SOUND_H
