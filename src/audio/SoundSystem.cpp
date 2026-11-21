#include "audio/SoundSystem.h"
#include <cmath>

using namespace std;

SoundSystem::SoundSystem() : muted_(false) {
    // little generated beeps, no extra sound files or libraries
    const int sampleRate = 22050;
    const int sampleCount = 11025;
    sf::Int16 samples[sampleCount];
    const float notes[CUE_COUNT] = {660.f, 440.f, 220.f, 780.f, 520.f, 880.f, 660.f};
    for (int cue = 0; cue < CUE_COUNT; ++cue) {
        float phase = 0.f;
        for (int i = 0; i < sampleCount; ++i) {
            const float t = static_cast<float>(i) / sampleCount;
            float frequency = notes[cue];
            if (cue == static_cast<int>(SoundCue::Jump)) frequency += 450.f * t;
            if (cue == static_cast<int>(SoundCue::Death)) frequency *= 1.f - 0.8f * t;
            if (cue >= static_cast<int>(SoundCue::Chapter)) {
                frequency *= 1.f + static_cast<int>(t * 4.f) * 0.25f;
            }
            phase += 6.2831853f * frequency / sampleRate;
            float volume = (1.f - t) * 0.18f;
            if (i < 150) volume *= static_cast<float>(i) / 150.f;
            samples[i] = static_cast<sf::Int16>(sin(phase) * volume * 32767.f);
        }
        ready_[cue] = buffers_[cue].loadFromSamples(samples, sampleCount, 1, sampleRate);
        if (ready_[cue]) sounds_[cue].setBuffer(buffers_[cue]);
    }
}

void SoundSystem::play(SoundCue cue) {
    const int index = static_cast<int>(cue);
    if (muted_ || index < 0 || index >= CUE_COUNT || !ready_[index]) return;
    sounds_[index].play();
}

void SoundSystem::toggleMute() {
    setMuted(!muted_);
}

void SoundSystem::setMuted(bool muted) {
    muted_ = muted;
    if (muted_) stopAll();
}

bool SoundSystem::isMuted() const { return muted_; }

void SoundSystem::stopAll() {
    for (int i = 0; i < CUE_COUNT; ++i) sounds_[i].stop();
}
