#pragma once

#include "core/Position.h"
#include <SFML/Graphics.hpp>

using namespace std;

// visual ghost only, hussain can use its bounds for switches later
class GhostReplay {
public:
    GhostReplay();
    explicit GhostReplay(const Position& start);

    void reset();
    void setPosition(const Position& position);
    void setVisible(bool visible);
    bool isVisible() const;
    const Position& getPosition() const;
    sf::FloatRect getBounds() const;
    void draw(sf::RenderTarget& target) const;

private:
    Position position_;
    bool visible_;
    sf::RectangleShape body_;
};
