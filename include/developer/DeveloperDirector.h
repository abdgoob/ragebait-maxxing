#pragma once

#include "traps/Trap.h"
#include <string>
#include <vector>

class Player;
class Level;
class PatchManager;

class DeveloperDirector {
public:
    DeveloperDirector();

    void onPlayerDeath(Player& player, const Trap* trap);
    void onLevelStart(const Level& level);
    void onRetry(Player& player);

    std::string getLastTaunt() const;
    std::string getLastPatchReason() const;
    int getMood() const;

    void maybeDropPatch(Player& player, Level& level, PatchManager& pm);

private:
    std::string pickTaunt(TrapKind kind) const;
    void adjustMood(bool playerDied);

    int mood_;
    int deathsThisLevel_;
    std::string lastTaunt_;
    std::string lastPatchReason_;
    std::vector<std::string> genericTaunts_;
};