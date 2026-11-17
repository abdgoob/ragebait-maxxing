#pragma once

#include "traps/Trap.h"
#include <memory>
#include <string>

using namespace std;

// factory bc proposal said so and its easier than new everywhere

class TrapFactory {
public:
    static unique_ptr<Trap> create(TrapKind kind, const Position& pos);

    // string version for level files later maybe
    static unique_ptr<Trap> createFromTag(const string& tag, const Position& pos);
};