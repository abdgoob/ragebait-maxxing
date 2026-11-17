#pragma once

#include "traps/Trap.h"
#include <string>

using namespace std;

// factory bc proposal said so and its easier than new everywhere

class TrapFactory {
public:
    static Trap* create(TrapKind kind, const Position& pos);
    static Trap* createFromTag(const string& tag, const Position& pos);
};