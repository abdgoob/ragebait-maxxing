#pragma once

#include "core/GameObject.h"

using namespace std;

// flag on a pole, 32 wide and 56 tall, put its y at the floor top minus 56
// real ones move your respawn, fake ones glow nicely, say progress saved and do nothing

class Checkpoint : public GameObject {
public:
    Checkpoint();
    explicit Checkpoint(const Position& pos);
    Checkpoint(const Position& pos, bool fake);

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;

    bool touches(const sf::FloatRect& box) const;
    bool isFake() const;
    bool isReached() const;
    void markReached();
    // where the floor is under the flag, the player respawns standing here
    float getFloorY() const;

    sf::FloatRect getBounds() const;

private:
    bool fake_;
    bool reached_;
    float glow_;
    sf::RectangleShape pole_;
    sf::RectangleShape flag_;
};
