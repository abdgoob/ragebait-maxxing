#include "traps/TrapFactory.h"
#include "traps/ControlTrap.h"
#include "traps/FakePlatform.h"
#include "traps/SpikeTrap.h"

using namespace std;

Trap* TrapFactory::create(TrapKind kind, const Position& pos) {
    if (kind == TrapKind::Spike) {
        return new SpikeTrap(pos, 70.f, 3.f);
    }
    if (kind == TrapKind::FakePlatform) {
        return new FakePlatform(pos, 110.f, 18.f);
    }
    if (kind == TrapKind::ControlSwap) {
        return new ControlTrap(pos, 40.f, 40.f);
    }
    return new SpikeTrap(pos, 50.f, 2.f);
}

Trap* TrapFactory::createFromTag(const string& tag, const Position& pos) {
    if (tag == "spike") return create(TrapKind::Spike, pos);
    if (tag == "fake_floor" || tag == "fake") return create(TrapKind::FakePlatform, pos);
    if (tag == "ctrl" || tag == "control") return create(TrapKind::ControlSwap, pos);
    return create(TrapKind::Spike, pos);
}