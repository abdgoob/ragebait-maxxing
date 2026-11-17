#pragma once

#include "patches/PatchEffect.h"

using namespace std;

class ControlPatch : public PatchEffect {
public:
    ControlPatch();

    void apply(Player& player, Level& level) override;
    void revert(Player& player, Level& level) override;

    string getVersion() const override;
    string getAnnouncement() const override;
    bool isTimed() const override;
    float getDuration() const override;

private:
    bool wasFlipped_;
};