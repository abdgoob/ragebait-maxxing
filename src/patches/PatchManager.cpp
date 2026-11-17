#include "patches/PatchManager.h"
#include "patches/ControlPatch.h"
#include "patches/GravityPatch.h"
#include "stubs/Level.h"
#include "stubs/Player.h"

using namespace std;

PatchManager::PatchManager() : activeCount_(0), patchTimer_(0.f) {
    for (int i = 0; i < MAX_ACTIVE; ++i) {
        active_[i].reset();
    }
}

void PatchManager::queuePatch(unique_ptr<PatchEffect> patch) {
    pending_ = move(patch);
}

void PatchManager::applyQueued(Player& player, Level& level) {
    if (!pending_ || activeCount_ >= MAX_ACTIVE) return;

    pending_->apply(player, level);
    if (pending_->isTimed()) {
        patchTimer_ = pending_->getDuration();
    }
    active_[activeCount_] = move(pending_);
    ++activeCount_;
}

void PatchManager::update(float dt, Player& player, Level& level) {
    if (pending_) {
        applyQueued(player, level);
    }

    if (patchTimer_ > 0.f) {
        patchTimer_ -= dt;
        if (patchTimer_ <= 0.f) {
            for (int i = 0; i < activeCount_; ++i) {
                if (active_[i] && active_[i]->isActive()) {
                    active_[i]->revert(player, level);
                }
                active_[i].reset();
            }
            activeCount_ = 0;
        }
    }
}

bool PatchManager::hasPending() const { return pending_ != nullptr; }

string PatchManager::peekNextAnnouncement() const {
    if (!pending_) return "";
    return pending_->getAnnouncement();
}

void PatchManager::clearAll(Player& player, Level& level) {
    for (int i = 0; i < activeCount_; ++i) {
        if (active_[i]) active_[i]->revert(player, level);
        active_[i].reset();
    }
    activeCount_ = 0;
    pending_.reset();
    patchTimer_ = 0.f;
}