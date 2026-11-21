#include "game/GhostReplay.h"

using namespace std;

GhostReplay::GhostReplay() : GhostReplay(Position()) {}

GhostReplay::GhostReplay(const Position& start) : position_(start), visible_(false) {
    body_.setSize(sf::Vector2f(28.f, 36.f));
    body_.setFillColor(sf::Color(130, 210, 255, 95));
    body_.setOutlineColor(sf::Color(180, 235, 255, 150));
    body_.setOutlineThickness(1.f);
    body_.setPosition(position_.getX(), position_.getY());
}

void GhostReplay::reset() {
    position_ = Position();
    visible_ = false;
    body_.setPosition(0.f, 0.f);
}

void GhostReplay::setPosition(const Position& position) {
    position_ = position;
    body_.setPosition(position_.getX(), position_.getY());
}

void GhostReplay::setVisible(bool visible) { visible_ = visible; }
bool GhostReplay::isVisible() const { return visible_; }
const Position& GhostReplay::getPosition() const { return position_; }
sf::FloatRect GhostReplay::getBounds() const {
    return sf::FloatRect(position_.getX(), position_.getY(), 28.f, 36.f);
}

void GhostReplay::draw(sf::RenderTarget& target) const {
    if (visible_) target.draw(body_);
}
