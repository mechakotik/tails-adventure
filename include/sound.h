#ifndef TA_SOUND_H
#define TA_SOUND_H

#include <string>
#include "SDL3_mixer/SDL_mixer.h"

enum TA_SoundChannel
{
    TA_SOUND_CHANNEL_SFX1,
    TA_SOUND_CHANNEL_SFX2,
    TA_SOUND_CHANNEL_SFX3,
    TA_SOUND_CHANNEL_MAX
};

namespace TA::sound {
    void playMusic(std::string filename, int repeat = -1);
    void update();
    bool isPlaying(TA_SoundChannel channel);
    bool isMusicPlaying();
    void fadeOut(int time);
    void fadeOutMusic(int time);
    void fadeOutChannel(TA_SoundChannel channel, int time);
}

class TA_Sound
{
private:
    Mix_Chunk *chunk = nullptr;
    TA_SoundChannel channel = TA_SOUND_CHANNEL_SFX1;
    bool loop = false;

public:
    void load(std::string filename, TA_SoundChannel channel, bool loop = false);
    void play();
    void fadeOut(int time);
    void clear() {chunk = nullptr;}
    bool empty() {return chunk == nullptr;}
};

#endif // TA_SOUND_H
