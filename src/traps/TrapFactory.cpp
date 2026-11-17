#include "traps/TrapFactory.h"
#include "traps/ControlTrap.h"
#include "traps/FakePlatform.h"
#include "traps/SpikeTrap.h"

std::unique_ptr<Trap> TrapFactory::create(TrapKind kind, const Position& pos) {
    switch (kind) {
        case TrapKind::Spike:
            return std::make_unique<SpikeTrap>(pos, 70.f, 3.f);
        case TrapKind::FakePlatform:
            return std::make_unique<FakePlatform>(pos, 110.f, 18.f);
        case TrapKind::ControlSwap:
            return std::make_unique<ControlTrap>(pos, 40.f, 40.f);
    }
    return std::make_unique<SpikeTrap>(pos, 50.f, 2.f);
}

std::unique_ptr<Trap> TrapFactory::createFromTag(const std::string& tag, const Position& pos) {
    if (tag == "spike") return create(TrapKind::Spike, pos);
    if (tag == "fake_floor" || tag == "fake") return create(TrapKind::FakePlatform, pos);
    if (tag == "ctrl" || tag == "control") return create(TrapKind::ControlSwap, pos);
    return create(TrapKind::Spike, pos);
}