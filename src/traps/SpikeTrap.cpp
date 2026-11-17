#include "traps/SpikeTrap.h"
#include "stubs/Player.h"

#include <cmath>

using namespace std;

SpikeTrap::SpikeTrap(const Position& pos, float moveDistance, float speed)
    : Trap(pos, TrapKind::Spike),
      startX_(pos.getX()),
      moveDistance_(moveDistance),
      speed_(speed),
      t_(0.f),
      goingRight_(true) {
    shape_.setFillColor(sf::Color(220, 50, 90));
    shape_.setSize(sf::Vector2f(56.f, 20.f));
}

bool SpikeTrap::hurtsPlayer(const Player& player) const {
    if (!active_) return false;
    const sf::FloatRect pb = player.getBounds();
    const sf::FloatRect tb = shape_.getGlobalBounds();
    return pb.intersects(tb);
}

void SpikeTrap::update(float dt) {
    t_ += dt;
    float offset = sin(t_ * speed_) * moveDistance_;
    pos_.setX(startX_ + offset);
    shape_.setPosition(pos_.getX(), pos_.getY());
}

void SpikeTrap::draw(sf::RenderTarget& target) const {
    if (!active_) return;
    target.draw(shape_);
}

void SpikeTrap::reset() {
    t_ = 0.f;
    pos_.setX(startX_);
    active_ = true;
}