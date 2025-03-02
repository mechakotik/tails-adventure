#include "sound.h"
#include "error.h"
#include "resource_manager.h"
#include "save.h"

void TA::sound::playMusic(std::string filename, int repeat) {
    Mix_Music* music = TA::resmgr::loadMusic(filename);
    Mix_PlayMusic(music, repeat);
}

void TA::sound::update() {
    Mix_MasterVolume(TA::save::getParameter("main_volume") * 16);
    Mix_VolumeMusic(TA::save::getParameter("music_volume") * 8);
    Mix_Volume(TA_SOUND_CHANNEL_SFX1, TA::save::getParameter("sfx_volume") * 16);
    Mix_Volume(TA_SOUND_CHANNEL_SFX2, TA::save::getParameter("sfx_volume") * 16);
    Mix_Volume(TA_SOUND_CHANNEL_SFX3, TA::save::getParameter("sfx_volume") * 16);
}

bool TA::sound::isPlaying(TA_SoundChannel channel) {
    return Mix_Playing(channel);
}

bool TA::sound::isMusicPlaying() {
    return Mix_PlayingMusic();
}

void TA::sound::fadeOut(int time) {
    fadeOutMusic(time);
    for(int channel = 0; channel < TA_SOUND_CHANNEL_MAX; channel++) {
        fadeOutChannel(TA_SoundChannel(channel), time);
    }
}

void TA::sound::fadeOutMusic(int time) {
    Mix_FadeOutMusic(time * 1000 / 60);
}

void TA::sound::fadeOutChannel(TA_SoundChannel channel, int time) {
    Mix_FadeOutChannel(channel, time * 1000 / 60);
}

void TA_Sound::load(std::string filename, TA_SoundChannel newChannel, bool newLoop) {
    chunk = TA::resmgr::loadChunk(filename);
    channel = newChannel;
    loop = newLoop;
}

void TA_Sound::play() {
    if(chunk == nullptr) {
        return;
    }
    Mix_PlayChannel(channel, chunk, loop);
}

void TA_Sound::fadeOut(int time) {
    Mix_FadeOutChannel(channel, time * 1000 / 60);
}
