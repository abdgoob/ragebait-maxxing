#include "patches/PatchEffect.h"

using namespace std;

bool PatchEffect::isTimed() const { return false; }

float PatchEffect::getDuration() const { return 0.f; }

bool PatchEffect::isActive() const { return active_; }

void PatchEffect::markActive(bool on) { active_ = on; }