#pragma once

#include "world/Platform.h"

using namespace std;

// slides from its start to start + travel and back again, forever
// level carries the player if they are standing on it

class MovingPlatform : public Platform {
public:
    MovingPlatform();
    MovingPlatform(const Position& start, float width, float height, const Position& travel, float speed);

    void update(float dt) override;
    void reset() override;

private:
    Position start_;
    Position travel_;
    float speed_;     // pixels per second
    float progress_;  // 0 at the start, 1 at the far end
    int dir_;
};
