#pragma once

#include "traps/Trap.h"

class ControlTrap : public Trap {
public:
    ControlTrap(const Position& pos, float width, float height);

    bool hurtsPlayer(const Player& player) const override;
    void onPlayerTouch(Player& player) override;
    void draw(sf::RenderTarget& target) const override;
    void reset() override;

private:
    bool alreadyHit_;
};