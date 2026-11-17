#pragma once

#include "traps/Trap.h"
#include <string>

using namespace std;

class Player;
class Level;
class PatchManager;

class DeveloperDirector {
public:
    static const int TAUNT_COUNT = 5;

    DeveloperDirector();

    void onPlayerDeath(Player& player, const Trap* trap);
    void onLevelStart(const Level& level);
    void onRetry(Player& player);

    string getLastTaunt() const;
    string getLastPatchReason() const;
    int getMood() const;

    void maybeDropPatch(Player& player, Level& level, PatchManager& pm);

private:
    string pickTaunt(TrapKind kind) const;
    void adjustMood(bool playerDied);

    int mood_;
    int deathsThisLevel_;
    string lastTaunt_;
    string lastPatchReason_;
    string genericTaunts_[TAUNT_COUNT];
};