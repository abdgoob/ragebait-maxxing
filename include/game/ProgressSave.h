#pragma once

#include <string>

using namespace std;

// small text save, one slot for each of the six chapters
class ProgressSave {
public:
    static const int MAX_CHAPTERS = 6;

    ProgressSave();

    void reset();
    bool load(const string& path);
    bool save(const string& path) const;

    void unlockChapter(int chapter);
    void completeChapter(int chapter, int deaths);
    bool isUnlocked(int chapter) const;
    bool isCompleted(int chapter) const;
    int getBestDeaths(int chapter) const;
    int getHighestUnlocked() const;

    void setMuted(bool muted);
    bool isMuted() const;
    void addDeath();
    void addRetry();
    int getTotalDeaths() const;
    int getTotalRetries() const;

private:
    int highestUnlocked_;
    bool completed_[MAX_CHAPTERS];
    int bestDeaths_[MAX_CHAPTERS];
    bool muted_;
    int totalDeaths_;
    int totalRetries_;
};
