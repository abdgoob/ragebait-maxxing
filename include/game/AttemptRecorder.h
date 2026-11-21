#pragma once

#include "core/Position.h"

using namespace std;

// records positions only, hussain still owns movement and switch collisions
class AttemptRecorder {
public:
    static const int MAX_FRAMES = 7200;

    AttemptRecorder();
    void clear();
    void startAttempt();
    void startAttempt(const Position& start);
    void record(const Position& position);
    void record(float dt, const Position& position);
    void finishAttempt();
    void updateReplay(float dt);

    bool hasGhost() const;
    const Position& getGhostPosition() const;
    bool isRecordingFull() const;

private:
    struct Frame {
        Position position;
        float time;
    };

    Frame current_[MAX_FRAMES];
    Frame previous_[MAX_FRAMES];
    int currentCount_;
    int previousCount_;
    int replayIndex_;
    float recordTime_;
    float replayTime_;
    Position ghost_;
};
