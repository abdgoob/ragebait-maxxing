#pragma once

using namespace std;

// numbers for the current run, save data keeps the lifetime totals
class RunSummary {
public:
    RunSummary();

    void reset();
    void startChapter();
    void addDeath();
    void addRetry();
    void addChapterClear();
    void update(float dt);

    int getRunDeaths() const;
    int getChapterDeaths() const;
    int getRetries() const;
    int getChaptersCleared() const;
    float getPlayTime() const;

private:
    int runDeaths_;
    int chapterDeaths_;
    int retries_;
    int chaptersCleared_;
    float playTime_;
};
