#include "traps/TrapFactory.h"
#include "traps/ControlTrap.h"
#include "traps/FakePlatform.h"
#include "traps/SpikeTrap.h"

using namespace std;

Trap* TrapFactory::create(TrapKind kind, const Position& pos) {
    if (kind == TrapKind::Spike) return makeSpike(pos, 70.f, 3.f);
    if (kind == TrapKind::FakePlatform) return makeFakeFloor(pos, 110.f, 24.f);
    if (kind == TrapKind::ControlSwap) return makeCtrlPad(pos);
    return makeSpike(pos, 50.f, 2.f);
}

Trap* TrapFactory::createFromTag(const string& tag, const Position& pos) {
    if (tag == "spike") return create(TrapKind::Spike, pos);
    if (tag == "fake_floor" || tag == "fake") return create(TrapKind::FakePlatform, pos);
    if (tag == "ctrl" || tag == "control") return create(TrapKind::ControlSwap, pos);
    return create(TrapKind::Spike, pos);
}

Trap* TrapFactory::makeSpike(const Position& pos, float moveDist, float speed) {
    return new SpikeTrap(pos, moveDist, speed);
}

Trap* TrapFactory::makeFakeFloor(const Position& pos, float w, float h) {
    return new FakePlatform(pos, w, h);
}

Trap* TrapFactory::makeCtrlPad(const Position& pos) {
    return new ControlTrap(pos, 44.f, 16.f);
}