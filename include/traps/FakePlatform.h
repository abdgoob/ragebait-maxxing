#pragma once

#include "traps/Trap.h"

using namespace std;

class FakePlatform : public Trap {
public:
    FakePlatform(const Position& pos, float width, float height);

    bool hurtsPlayer(const Player& player) const override;
    void onPlayerTouch(Player& player) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;
    void reset() override;

    bool isFallen() const;

private:
    float w_;
    float h_;
    bool fallen_;
    float fallTimer_;
};