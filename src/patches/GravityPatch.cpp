#include "patches/GravityPatch.h"
#include "stubs/Level.h"
#include "stubs/Player.h"

using namespace std;

GravityPatch::GravityPatch(float newGravity)
    : newGravity_(newGravity), oldGravity_(900.f), playerGravityBackup_(1.f) {}

void GravityPatch::apply(Player& player, Level& level) {
    oldGravity_ = level.getBaseGravity();
    playerGravityBackup_ = player.getGravityScale();

    level.setBaseGravity(newGravity_);
    player.setGravityScale(newGravity_ / 900.f);
    markActive(true);
}

void GravityPatch::revert(Player& player, Level& level) {
    level.setBaseGravity(oldGravity_);
    player.setGravityScale(playerGravityBackup_);
    markActive(false);
}

string GravityPatch::getVersion() const { return "v0.0.3-hotfix"; }

string GravityPatch::getAnnouncement() const {
    return "PATCH NOTES: gravity feels 'more realistic' now :)";
}

bool GravityPatch::isTimed() const { return true; }
float GravityPatch::getDuration() const { return 8.f; }