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
    if (level.getName() == "04 unpaid intern") lastTaunt_ = "need help? record yourself on the plate, then retry.";
    if (level.getName() == "05 accept all cookies") lastTaunt_ = "that popup is load bearing. drag it over the gap.";
    if (level.getName() == "06 the final hotfix") lastTaunt_ = "i marked where im about to delete you. move.";
    mood_ = 3;
}

void DeveloperDirector::onRetry(Player& player) {
    player.respawn();
    const char* retries[3] = { "same plan again? bold research method.",
        "your last attempt is now a cautionary tale.", "respawned. the floor has requested a rematch." };
    lastTaunt_ = retries[deathsThisLevel_ % 3];
}

void DeveloperDirector::onFakeCheckpoint() {
    lastTaunt_ = "progress saved*  *in my imagination. skill issue.";
}

void DeveloperDirector::onFakeExit(int chapter) {
    if (chapter == 1) lastTaunt_ = "congrats! your floor subscription just expired. jump.";
    else if (chapter == 2) lastTaunt_ = "wrong door. spikes installed. this is clearly your fault.";
    else if (chapter == 3) lastTaunt_ = "victory rolled back. left is right. thanks for beta testing.";
    else if (chapter == 4) lastTaunt_ = "your ghost did the work. i moved the exit anyway.";
    else lastTaunt_ = "thanks for accepting the cookies. actual door is further right.";
    mood_ = 5;
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
            return "the spikes filed a complaint. stop landing on them.";
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
