#include "game/RunSummary.h"

using namespace std;

RunSummary::RunSummary() { reset(); }

void RunSummary::reset() {
    runDeaths_ = 0;
    chapterDeaths_ = 0;
    retries_ = 0;
    chaptersCleared_ = 0;
    playTime_ = 0.f;
}

void RunSummary::startChapter() { chapterDeaths_ = 0; }
void RunSummary::addDeath() { ++runDeaths_; ++chapterDeaths_; }
void RunSummary::addRetry() { ++retries_; }
void RunSummary::addChapterClear() { ++chaptersCleared_; }
void RunSummary::update(float dt) { if (dt > 0.f) playTime_ += dt; }
int RunSummary::getRunDeaths() const { return runDeaths_; }
int RunSummary::getChapterDeaths() const { return chapterDeaths_; }
int RunSummary::getRetries() const { return retries_; }
int RunSummary::getChaptersCleared() const { return chaptersCleared_; }
float RunSummary::getPlayTime() const { return playTime_; }
