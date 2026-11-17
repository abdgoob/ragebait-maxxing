#pragma once

#include "patches/PatchEffect.h"

class ControlPatch : public PatchEffect {
public:
    ControlPatch();

    void apply(Player& player, Level& level) override;
    void revert(Player& player, Level& level) override;

    std::string getVersion() const override;
    std::string getAnnouncement() const override;
    bool isTimed() const override;
    float getDuration() const override;

private:
    bool wasFlipped_;
};