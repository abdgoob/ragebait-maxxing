#include "patches/PatchManager.h"
#include "stubs/Level.h"
#include "stubs/Player.h"

using namespace std;

PatchManager::PatchManager() : activeCount_(0), pending_(0), patchTimer_(0.f) {
    for (int i = 0; i < MAX_ACTIVE; ++i) {
        active_[i] = 0;
    }
}

PatchManager::~PatchManager() {
    freeActive();
    if (pending_) {
        delete pending_;
        pending_ = 0;
    }
}

void PatchManager::freeActive() {
    for (int i = 0; i < activeCount_; ++i) {
        delete active_[i];
        active_[i] = 0;
    }
    activeCount_ = 0;
}

void PatchManager::queuePatch(PatchEffect* patch) {
    pending_ = patch;
}

void PatchManager::applyQueued(Player& player, Level& level) {
    if (!pending_ || activeCount_ >= MAX_ACTIVE) return;

    pending_->apply(player, level);
    if (pending_->isTimed()) {
        patchTimer_ = pending_->getDuration();
    }
    active_[activeCount_] = pending_;
    ++activeCount_;
    pending_ = 0;
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
            }
            freeActive();
        }
    }
}

bool PatchManager::hasPending() const { return pending_ != 0; }

string PatchManager::peekNextAnnouncement() const {
    if (!pending_) return "";
    return pending_->getAnnouncement();
}

void PatchManager::clearAll(Player& player, Level& level) {
    for (int i = 0; i < activeCount_; ++i) {
        if (active_[i]) {
            active_[i]->revert(player, level);
            delete active_[i];
            active_[i] = 0;
        }
    }
    activeCount_ = 0;

    if (pending_) {
        delete pending_;
        pending_ = 0;
    }
    patchTimer_ = 0.f;
}