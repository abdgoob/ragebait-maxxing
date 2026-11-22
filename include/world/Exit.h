#pragma once

#include "core/GameObject.h"

using namespace std;

// green door at the end of a chapter
// a fake one looks exactly the same, fools you once, then disappears until retry

class Exit : public GameObject {
public:
    Exit();
    Exit(const Position& pos, float width, float height);
    Exit(const Position& pos, float width, float height, bool fake);

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;

    bool isReachedBy(const sf::FloatRect& box) const;
    bool isReachedBy(const Player& player) const;
    bool isFake() const;
    void use();
    void reset();

    // the boss can take the exit away
    void setEnabled(bool on);
    bool isEnabled() const;

    sf::FloatRect getBounds() const;

private:
    float w_;
    float h_;
    bool fake_;
    bool used_;
    bool enabled_;
    float pulse_;
    sf::RectangleShape frame_;
    sf::RectangleShape door_;
};
