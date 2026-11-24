#pragma once

#include "traps/Trap.h"
#include <string>

using namespace std;

class Player;
class Level;
class PatchManager;

class DeveloperDirector {
public:
    static const int TAUNT_COUNT = 8;

    DeveloperDirector();

    void setPatchHappy(bool on);

    void onPlayerDeath(Player& player, const Trap* trap);
    void onLevelStart(const Level& level);
    void onRetry(Player& player);
    void onFakeCheckpoint();
    void onFakeExit(int chapter);

    string getLastTaunt() const;
    string getLastPatchReason() const;
    int getMood() const;

    void maybeDropPatch(Player& player, Level& level, PatchManager& pm);

private:
    string pickTaunt(TrapKind kind) const;
    void adjustMood(bool playerDied);

    int mood_;
    int deathsThisLevel_;
    bool patchHappy_;
    string lastTaunt_;
    string lastPatchReason_;
    string genericTaunts_[TAUNT_COUNT];
};
