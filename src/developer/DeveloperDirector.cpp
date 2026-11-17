#include "developer/DeveloperDirector.h"
#include "patches/ControlPatch.h"
#include "patches/GravityPatch.h"
#include "patches/PatchManager.h"
#include "stubs/Level.h"
#include "stubs/Player.h"

DeveloperDirector::DeveloperDirector() : mood_(3), deathsThisLevel_(0) {
    genericTaunts_ = {
        "skill issue tbh",
        "working as intended",
        "have you tried being good at the game",
        "logged your death for analytics",
        "this trap is actually fair if you think about it"};
}

void DeveloperDirector::onPlayerDeath(Player& player, const Trap* trap) {
    (void)player;
    ++deathsThisLevel_;
    adjustMood(true);

    if (trap) {
        lastTaunt_ = pickTaunt(trap->getKind());
    } else {
        lastTaunt_ = genericTaunts_[deathsThisLevel_ % genericTaunts_.size()];
    }
}

void DeveloperDirector::onLevelStart(const Level& level) {
    deathsThisLevel_ = 0;
    lastTaunt_ = "welcome to " + level.getName() + " (no refunds)";
    mood_ = 3;
}

void DeveloperDirector::onRetry(Player& player) {
    player.respawn(player.getSpawnPoint());
    lastTaunt_ = "respawned. try not to embarrass us both";
}

std::string DeveloperDirector::getLastTaunt() const { return lastTaunt_; }

std::string DeveloperDirector::getLastPatchReason() const { return lastPatchReason_; }

int DeveloperDirector::getMood() const { return mood_; }

void DeveloperDirector::maybeDropPatch(Player& player, Level& level, PatchManager& pm) {
    (void)player;

    if (deathsThisLevel_ > 0 && deathsThisLevel_ % 2 == 0 && !pm.hasPending()) {
        if (deathsThisLevel_ % 4 == 0) {
            lastPatchReason_ = "you keep dying so im changing gravity";
            pm.queuePatch(std::make_unique<GravityPatch>(1350.f));
        } else {
            lastPatchReason_ = "controls felt too intuitive";
            pm.queuePatch(std::make_unique<ControlPatch>());
        }
    }
}

std::string DeveloperDirector::pickTaunt(TrapKind kind) const {
    switch (kind) {
        case TrapKind::Spike:
            return "did the moving spike look friendly?";
        case TrapKind::FakePlatform:
            return "green means safe right? lol";
        case TrapKind::ControlSwap:
            return "your keyboard might be broken (its not)";
    }
    return genericTaunts_[0];
}

void DeveloperDirector::adjustMood(bool playerDied) {
    if (playerDied && mood_ < 5) ++mood_;
    if (!playerDied && mood_ > 1) --mood_;
}