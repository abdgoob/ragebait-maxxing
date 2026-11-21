#include "game/AttemptRecorder.h"

using namespace std;

AttemptRecorder::AttemptRecorder() { clear(); }

void AttemptRecorder::clear() {
    currentCount_ = 0;
    previousCount_ = 0;
    replayIndex_ = 0;
    recordTime_ = 0.f;
    replayTime_ = 0.f;
    ghost_ = Position();
}

void AttemptRecorder::startAttempt() { startAttempt(Position()); }

void AttemptRecorder::startAttempt(const Position& start) {
    currentCount_ = 1;
    current_[0].position = start;
    current_[0].time = 0.f;
    recordTime_ = 0.f;
    replayTime_ = 0.f;
    replayIndex_ = 0;
    if (previousCount_ > 0) ghost_ = previous_[0].position;
}

void AttemptRecorder::record(float dt, const Position& position) {
    if (dt <= 0.f || currentCount_ >= MAX_FRAMES) return;
    recordTime_ += dt;
    current_[currentCount_].position = position;
    current_[currentCount_].time = recordTime_;
    ++currentCount_;
}

void AttemptRecorder::record(const Position& position) {
    record(1.f / 60.f, position);
}

void AttemptRecorder::finishAttempt() {
    previousCount_ = currentCount_;
    for (int i = 0; i < currentCount_; ++i) previous_[i] = current_[i];
}

void AttemptRecorder::updateReplay(float dt) {
    if (dt <= 0.f || !hasGhost()) return;
    replayTime_ += dt;
    while (replayIndex_ + 1 < previousCount_ && previous_[replayIndex_ + 1].time <= replayTime_) {
        ++replayIndex_;
    }
    ghost_ = previous_[replayIndex_].position;
    if (replayIndex_ + 1 >= previousCount_) return;
    const Frame& a = previous_[replayIndex_];
    const Frame& b = previous_[replayIndex_ + 1];
    const float span = b.time - a.time;
    if (span <= 0.f) return;
    const float blend = (replayTime_ - a.time) / span;
    ghost_ = Position(a.position.getX() + (b.position.getX() - a.position.getX()) * blend,
                      a.position.getY() + (b.position.getY() - a.position.getY()) * blend);
}

bool AttemptRecorder::hasGhost() const { return previousCount_ > 1; }
const Position& AttemptRecorder::getGhostPosition() const { return ghost_; }
bool AttemptRecorder::isRecordingFull() const { return currentCount_ >= MAX_FRAMES; }
