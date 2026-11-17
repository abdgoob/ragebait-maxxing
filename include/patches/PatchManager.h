#pragma once

#include "patches/PatchEffect.h"
#include <memory>
#include <string>

using namespace std;

class Player;
class Level;

class PatchManager {
public:
    static const int MAX_ACTIVE = 8;

    PatchManager();

    void queuePatch(unique_ptr<PatchEffect> patch);
    void applyQueued(Player& player, Level& level);
    void update(float dt, Player& player, Level& level);

    bool hasPending() const;
    string peekNextAnnouncement() const;

    void clearAll(Player& player, Level& level);

private:
    unique_ptr<PatchEffect> active_[MAX_ACTIVE];
    int activeCount_;
    unique_ptr<PatchEffect> pending_;
    float patchTimer_;
};