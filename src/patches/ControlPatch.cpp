#include "patches/ControlPatch.h"
#include "stubs/Level.h"
#include "stubs/Player.h"

using namespace std;

ControlPatch::ControlPatch() : wasFlipped_(false) {}

void ControlPatch::apply(Player& player, Level& level) {
    (void)level;
    wasFlipped_ = player.areControlsFlipped();
    player.setControlsFlipped(true);
    markActive(true);
}

void ControlPatch::revert(Player& player, Level& level) {
    (void)level;
    player.setControlsFlipped(wasFlipped_);
    markActive(false);
}

string ControlPatch::getVersion() const { return "v0.0.4-UX"; }

string ControlPatch::getAnnouncement() const {
    return "PATCH NOTES: improved movement clarity (left is right now)";
}

bool ControlPatch::isTimed() const { return true; }
float ControlPatch::getDuration() const { return 10.f; }