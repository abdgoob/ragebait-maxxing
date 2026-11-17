#include "patches/PatchManager.h"
#include "patches/ControlPatch.h"
#include "patches/GravityPatch.h"
#include "stubs/Level.h"
#include "stubs/Player.h"

PatchManager::PatchManager() : patchTimer_(0.f) {}

void PatchManager::queuePatch(std::unique_ptr<PatchEffect> patch) {
    pending_ = std::move(patch);
}

void PatchManager::applyQueued(Player& player, Level& level) {
    if (!pending_) return;

    pending_->apply(player, level);
    if (pending_->isTimed()) {
        patchTimer_ = pending_->getDuration();
    }
    active_.push_back(std::move(pending_));
}

void PatchManager::update(float dt, Player& player, Level& level) {
    if (pending_) {
        applyQueued(player, level);
    }

    if (patchTimer_ > 0.f) {
        patchTimer_ -= dt;
        if (patchTimer_ <= 0.f) {
            for (auto& p : active_) {
                if (p && p->isActive()) {
                    p->revert(player, level);
                }
            }
            active_.clear();
        }
    }
}

bool PatchManager::hasPending() const { return pending_ != nullptr; }

std::string PatchManager::peekNextAnnouncement() const {
    if (!pending_) return "";
    return pending_->getAnnouncement();
}

void PatchManager::clearAll(Player& player, Level& level) {
    for (auto& p : active_) {
        if (p) p->revert(player, level);
    }
    active_.clear();
    pending_.reset();
    patchTimer_ = 0.f;
}