#pragma once

#include "patches/PatchEffect.h"
#include <memory>
#include <vector>
#include <string>

class Player;
class Level;

class PatchManager {
public:
    PatchManager();

    void queuePatch(std::unique_ptr<PatchEffect> patch);
    void applyQueued(Player& player, Level& level);
    void update(float dt, Player& player, Level& level);

    bool hasPending() const;
    std::string peekNextAnnouncement() const;

    void clearAll(Player& player, Level& level);

private:
    std::vector<std::unique_ptr<PatchEffect>> active_;
    std::unique_ptr<PatchEffect> pending_;
    float patchTimer_;
};