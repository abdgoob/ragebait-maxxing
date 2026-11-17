#pragma once

#include "patches/PatchEffect.h"
#include <string>

using namespace std;

class Player;
class Level;

class PatchManager {
public:
    static const int MAX_ACTIVE = 8;

    PatchManager();
    ~PatchManager();

    void queuePatch(PatchEffect* patch);
    void applyQueued(Player& player, Level& level);
    void update(float dt, Player& player, Level& level);

    bool hasPending() const;
    string peekNextAnnouncement() const;

    void clearAll(Player& player, Level& level);

private:
    PatchEffect* active_[MAX_ACTIVE];
    int activeCount_;
    PatchEffect* pending_;
    float patchTimer_;

    void freeActive();
    PatchManager(const PatchManager& other);
    PatchManager& operator=(const PatchManager& other);
};
