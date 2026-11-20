#include "developer/DeveloperDirector.h"
#include "patches/ControlPatch.h"
#include "patches/GravityPatch.h"
#include "patches/PatchManager.h"
#include "stubs/Level.h"
#include "stubs/Player.h"

using namespace std;

DeveloperDirector::DeveloperDirector() : mood_(3), deathsThisLevel_(0), patchHappy_(false) {
    genericTaunts_[0] = "skill issue tbh";
    genericTaunts_[1] = "working as intended";
    genericTaunts_[2] = "have you tried being good at the game";
    genericTaunts_[3] = "logged your death for analytics";
    genericTaunts_[4] = "this trap is actually fair if you think about it";
    genericTaunts_[5] = "sent a crash report to nobody";
    genericTaunts_[6] = "maybe read the patch notes next time";
    genericTaunts_[7] = "we do not offer refunds";
}

void DeveloperDirector::setPatchHappy(bool on) { patchHappy_ = on; }

void DeveloperDirector::onPlayerDeath(Player& player, const Trap* trap) {
    (void)player;
    ++deathsThisLevel_;
    adjustMood(true);

    if (trap) {
        lastTaunt_ = pickTaunt(trap->getKind());
    } else {
        lastTaunt_ = genericTaunts_[deathsThisLevel_ % TAUNT_COUNT];
    }
}

void DeveloperDirector::onLevelStart(const Level& level) {
    deathsThisLevel_ = 0;
    lastTaunt_ = "welcome to " + level.getName() + " (no refunds)";
    mood_ = 3;
}

void DeveloperDirector::onRetry(Player& player) {
    player.respawn();
    lastTaunt_ = "respawned. try not to embarrass us both";
}

string DeveloperDirector::getLastTaunt() const { return lastTaunt_; }

string DeveloperDirector::getLastPatchReason() const { return lastPatchReason_; }

int DeveloperDirector::getMood() const { return mood_; }

void DeveloperDirector::maybeDropPatch(Player& player, Level& level, PatchManager& pm) {
    (void)player;
    (void)level;

    int every = patchHappy_ ? 1 : 2;
    if (deathsThisLevel_ > 0 && deathsThisLevel_ % every == 0 && !pm.hasPending()) {
        if (deathsThisLevel_ % 4 == 0) {
            lastPatchReason_ = "you keep dying so im changing gravity";
            pm.queuePatch(new GravityPatch(1300.f));
        } else {
            lastPatchReason_ = "controls felt too intuitive";
            pm.queuePatch(new ControlPatch());
        }
    }
}

string DeveloperDirector::pickTaunt(TrapKind kind) const {
    switch (kind) {
        case TrapKind::Spike:
            return "did the moving spike look friendly?";
        case TrapKind::FakePlatform:
            return "that floor was load bearing apparently";
        case TrapKind::ControlSwap:
            return "your keyboard might be broken (its not)";
    }
    return genericTaunts_[0];
}

void DeveloperDirector::adjustMood(bool playerDied) {
    if (playerDied && mood_ < 5) ++mood_;
    if (!playerDied && mood_ > 1) --mood_;
}