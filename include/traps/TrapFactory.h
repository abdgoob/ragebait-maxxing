#pragma once

#include "traps/Trap.h"
#include <memory>
#include <string>

// factory bc proposal said so and its easier than new everywhere

class TrapFactory {
public:
    static std::unique_ptr<Trap> create(TrapKind kind, const Position& pos);

    // string version for level files later maybe
    static std::unique_ptr<Trap> createFromTag(const std::string& tag, const Position& pos);
};