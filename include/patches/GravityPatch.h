#pragma once

#include "patches/PatchEffect.h"

using namespace std;

class GravityPatch : public PatchEffect {
public:
    explicit GravityPatch(float newGravity);

    void apply(Player& player, Level& level) override;
    void revert(Player& player, Level& level) override;

    string getVersion() const override;
    string getAnnouncement() const override;
    bool isTimed() const override;
    float getDuration() const override;

private:
    float newGravity_;
    float oldGravity_;
    float playerGravityBackup_;
};