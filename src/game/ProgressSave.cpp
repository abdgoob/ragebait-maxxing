#include "game/ProgressSave.h"
#include <fstream>

using namespace std;

ProgressSave::ProgressSave() { reset(); }

void ProgressSave::reset() {
    highestUnlocked_ = 1;
    muted_ = false;
    totalDeaths_ = 0;
    totalRetries_ = 0;
    for (int i = 0; i < MAX_CHAPTERS; ++i) {
        completed_[i] = false;
        bestDeaths_[i] = -1;
    }
}

bool ProgressSave::load(const string& path) {
    reset();
    ifstream file(path.c_str());
    if (!file.is_open()) return false;

    string name;
    int version = 0;
    ProgressSave loaded;
    if (!(file >> name >> version) || name != "RAGEBAIT_SAVE" || version != 1) return false;
    if (!(file >> name >> loaded.highestUnlocked_) || name != "unlocked") return false;
    int muted = 0;
    if (!(file >> name >> muted) || name != "muted") return false;
    if (!(file >> name >> loaded.totalDeaths_) || name != "deaths") return false;
    if (!(file >> name >> loaded.totalRetries_) || name != "retries") return false;
    if (loaded.highestUnlocked_ < 1 || loaded.highestUnlocked_ > MAX_CHAPTERS) return false;
    if ((muted != 0 && muted != 1) || loaded.totalDeaths_ < 0 || loaded.totalRetries_ < 0) return false;
    loaded.muted_ = muted == 1;

    for (int i = 0; i < MAX_CHAPTERS; ++i) {
        int chapter = 0;
        int completed = 0;
        int best = -1;
        if (!(file >> name >> chapter >> completed >> best) || name != "chapter") return false;
        if (chapter != i + 1 || (completed != 0 && completed != 1) || best < -1) return false;
        loaded.completed_[i] = completed == 1;
        loaded.bestDeaths_[i] = best;
    }

    *this = loaded;
    return true;
}

bool ProgressSave::save(const string& path) const {
    ofstream file(path.c_str());
    if (!file.is_open()) return false;
    file << "RAGEBAIT_SAVE 1\n";
    file << "unlocked " << highestUnlocked_ << "\n";
    file << "muted " << (muted_ ? 1 : 0) << "\n";
    file << "deaths " << totalDeaths_ << "\n";
    file << "retries " << totalRetries_ << "\n";
    for (int i = 0; i < MAX_CHAPTERS; ++i) {
        file << "chapter " << i + 1 << " " << (completed_[i] ? 1 : 0)
             << " " << bestDeaths_[i] << "\n";
    }
    return file.good();
}

void ProgressSave::unlockChapter(int chapter) {
    if (chapter < 1 || chapter > MAX_CHAPTERS) return;
    if (chapter > highestUnlocked_) highestUnlocked_ = chapter;
}

void ProgressSave::completeChapter(int chapter, int deaths) {
    if (chapter < 1 || chapter > MAX_CHAPTERS || deaths < 0) return;
    const int index = chapter - 1;
    completed_[index] = true;
    if (bestDeaths_[index] < 0 || deaths < bestDeaths_[index]) bestDeaths_[index] = deaths;
    if (chapter < MAX_CHAPTERS) unlockChapter(chapter + 1);
}

bool ProgressSave::isUnlocked(int chapter) const {
    return chapter >= 1 && chapter <= highestUnlocked_ && chapter <= MAX_CHAPTERS;
}

bool ProgressSave::isCompleted(int chapter) const {
    if (chapter < 1 || chapter > MAX_CHAPTERS) return false;
    return completed_[chapter - 1];
}

int ProgressSave::getBestDeaths(int chapter) const {
    if (chapter < 1 || chapter > MAX_CHAPTERS) return -1;
    return bestDeaths_[chapter - 1];
}

int ProgressSave::getHighestUnlocked() const { return highestUnlocked_; }
void ProgressSave::setMuted(bool muted) { muted_ = muted; }
bool ProgressSave::isMuted() const { return muted_; }
void ProgressSave::addDeath() { ++totalDeaths_; }
void ProgressSave::addRetry() { ++totalRetries_; }
int ProgressSave::getTotalDeaths() const { return totalDeaths_; }
int ProgressSave::getTotalRetries() const { return totalRetries_; }
