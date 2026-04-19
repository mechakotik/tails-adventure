#include "sound.h"
#include <array>
#include "error.h"
#include "resource_manager.h"
#include "save.h"

namespace {
    MIX_Mixer* mixer = nullptr;
    MIX_Track* musicTrack = nullptr;
    std::array<MIX_Track*, TA_SOUND_CHANNEL_MAX> channels{};

    float getVolumeGain(const std::string& parameter) {
        return static_cast<float>(TA::save::getParameter(parameter)) / 8.0F;
    }

    int64_t fadeFrames(MIX_Track* track, int time) {
        int64_t frames = MIX_TrackMSToFrames(track, static_cast<int64_t>(time) * 1000 / 60);
        return frames < 0 ? 0 : frames;
    }

    SDL_PropertiesID createLoopOptions(int loops) {
        if(loops == 0) {
            return 0;
        }

        SDL_PropertiesID options = SDL_CreateProperties();
        if(options == 0) {
            TA::handleSDLError("%s", "failed to create SDL_mixer playback options");
        }
        SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
        return options;
    }

    MIX_Track* getChannel(TA_SoundChannel channel) {
        return channels.at(channel);
    }
} // namespace

void TA::sound::init() {
    if(!MIX_Init()) {
        TA::handleSDLError("%s", "SDL_mixer init failed");
    }

    SDL_AudioSpec audioSpec;
    audioSpec.format = SDL_AUDIO_S16;
    audioSpec.channels = 2;
    audioSpec.freq = 44100;

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
    if(mixer == nullptr) {
        TA::handleSDLError("%s", "MIX_CreateMixerDevice failed");
    }

    musicTrack = MIX_CreateTrack(mixer);
    if(musicTrack == nullptr) {
        TA::handleSDLError("%s", "failed to create SDL_mixer music track");
    }

    for(MIX_Track*& channel : channels) {
        channel = MIX_CreateTrack(mixer);
        if(channel == nullptr) {
            TA::handleSDLError("%s", "failed to create SDL_mixer sound effect track");
        }
    }

    update();
}

void TA::sound::quit() {
    MIX_DestroyMixer(mixer);
    mixer = nullptr;
    musicTrack = nullptr;
    channels.fill(nullptr);
    MIX_Quit();
}

MIX_Mixer* TA::sound::getMixer() {
    return mixer;
}

void TA::sound::playMusic(const std::string& filename, int repeat) {
    MIX_Audio* music = TA::resmgr::loadMusic(filename);
    if(!MIX_SetTrackAudio(musicTrack, music)) {
        TA::handleSDLError("%s", "MIX_SetTrackAudio failed");
    }

    SDL_PropertiesID options = createLoopOptions(repeat);
    if(!MIX_PlayTrack(musicTrack, options)) {
        if(options != 0) {
            SDL_DestroyProperties(options);
        }
        TA::handleSDLError("%s", "MIX_PlayTrack failed");
    }
    if(options != 0) {
        SDL_DestroyProperties(options);
    }
}

void TA::sound::update() {
    MIX_SetMixerGain(mixer, getVolumeGain("main_volume"));
    MIX_SetTrackGain(musicTrack, getVolumeGain("music_volume"));
    for(MIX_Track* channel : channels) {
        MIX_SetTrackGain(channel, getVolumeGain("sfx_volume"));
    }
}

bool TA::sound::isPlaying(TA_SoundChannel channel) {
    return MIX_TrackPlaying(getChannel(channel));
}

bool TA::sound::isMusicPlaying() {
    return MIX_TrackPlaying(musicTrack);
}

void TA::sound::fadeOut(int time) {
    fadeOutMusic(time);
    for(int channel = 0; channel < TA_SOUND_CHANNEL_MAX; channel++) {
        fadeOutChannel(TA_SoundChannel(channel), time);
    }
}

void TA::sound::fadeOutMusic(int time) {
    MIX_StopTrack(musicTrack, fadeFrames(musicTrack, time));
}

void TA::sound::fadeOutChannel(TA_SoundChannel channel, int time) {
    MIX_StopTrack(getChannel(channel), fadeFrames(getChannel(channel), time));
}

void TA_Sound::load(const std::string& filename, TA_SoundChannel newChannel, bool newLoop) {
    chunk = TA::resmgr::loadChunk(filename);
    channel = newChannel;
    loop = newLoop;
}

void TA_Sound::play() {
    if(chunk == nullptr) {
        return;
    }
    MIX_Track* track = getChannel(channel);
    if(!MIX_SetTrackAudio(track, chunk)) {
        TA::handleSDLError("%s", "MIX_SetTrackAudio failed");
    }

    SDL_PropertiesID options = createLoopOptions(loop ? -1 : 0);
    if(!MIX_PlayTrack(track, options)) {
        if(options != 0) {
            SDL_DestroyProperties(options);
        }
        TA::handleSDLError("%s", "MIX_PlayTrack failed");
    }
    if(options != 0) {
        SDL_DestroyProperties(options);
    }
}

void TA_Sound::fadeOut(int time) {
    MIX_StopTrack(getChannel(channel), fadeFrames(getChannel(channel), time));
}
