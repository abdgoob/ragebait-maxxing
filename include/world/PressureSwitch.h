#pragma once

#include "core/GameObject.h"

using namespace std;

// flat plate on the floor, pressed while the player or the ghost stands on it
// it is 8px tall so put its y at the floor top minus 8

class PressureSwitch : public GameObject {
public:
    PressureSwitch();
    explicit PressureSwitch(const Position& pos);
    PressureSwitch(const Position& pos, float width);

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;

    bool touches(const sf::FloatRect& box) const;
    bool touches(const Player& player) const;

    void setPressed(bool pressed);
    bool isPressed() const;
    void reset();

    sf::FloatRect getBounds() const;

private:
    float w_;
    bool pressed_;
    sf::RectangleShape plate_;
};
