#ifndef TA_SOUND_H
#define TA_SOUND_H

#include <SDL3_mixer/SDL_mixer.h>
#include <string>

enum TA_SoundChannel { TA_SOUND_CHANNEL_SFX1, TA_SOUND_CHANNEL_SFX2, TA_SOUND_CHANNEL_SFX3, TA_SOUND_CHANNEL_MAX };

namespace TA::sound {
    void init();
    void quit();
    void playMusic(const std::string& filename, int repeat = -1);
    void update();
    bool isPlaying(TA_SoundChannel channel);
    bool isMusicPlaying();
    void fadeOut(int time);
    void fadeOutMusic(int time);
    void fadeOutChannel(TA_SoundChannel channel, int time);
    MIX_Mixer* getMixer();
} // namespace TA::sound

class TA_Sound {
private:
    MIX_Audio* chunk = nullptr;
    TA_SoundChannel channel = TA_SOUND_CHANNEL_SFX1;
    bool loop = false;

public:
    void load(const std::string& filename, TA_SoundChannel channel, bool loop = false);
    void play();
    void fadeOut(int time);
    void clear() { chunk = nullptr; }
    bool empty() { return chunk == nullptr; }
};

#endif // TA_SOUND_H
