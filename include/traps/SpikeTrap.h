#pragma once

#include "traps/Trap.h"

using namespace std;

class SpikeTrap : public Trap {
public:
    SpikeTrap(const Position& pos, float moveDistance, float speed);

    bool hurtsPlayer(const Player& player) const override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;
    void reset() override;

private:
    float startX_;
    float moveDistance_;
    float speed_;
    float t_;
    bool goingRight_;
};