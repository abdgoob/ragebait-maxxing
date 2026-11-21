#pragma once

#include <SFML/Audio.hpp>

using namespace std;

enum class SoundCue {
    Click,
    Jump,
    Death,
    Patch,
    Chapter,
    FakeWin,
    Locked,
    Save,
    Victory
};

class SoundSystem {
public:
    static const int CUE_COUNT = 9;

    SoundSystem();
    void play(SoundCue cue);
    void toggleMute();
    void setMuted(bool muted);
    bool isMuted() const;
    void stopAll();

private:
    // buffers stay alive longer than the sounds using them
    sf::SoundBuffer buffers_[CUE_COUNT];
    sf::Sound sounds_[CUE_COUNT];
    bool ready_[CUE_COUNT];
    bool muted_;

    SoundSystem(const SoundSystem& other);
    SoundSystem& operator=(const SoundSystem& other);
};
