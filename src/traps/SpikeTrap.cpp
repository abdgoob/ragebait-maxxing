#include "traps/SpikeTrap.h"
#include "stubs/Player.h"

#include <cmath>

using namespace std;

SpikeTrap::SpikeTrap(const Position& pos, float moveDistance, float speed)
    : Trap(pos, TrapKind::Spike),
      startX_(pos.getX()),
      startY_(pos.getY()),
      moveDistance_(moveDistance),
      speed_(speed),
      t_(0.f) {
    shape_.setFillColor(sf::Color(210, 40, 70));
    shape_.setSize(sf::Vector2f(56.f, 24.f));
    shape_.setPosition(pos_.getX(), pos_.getY());
}

bool SpikeTrap::hurtsPlayer(const Player& player) const {
    if (!active_) return false;
    return player.getBounds().intersects(shape_.getGlobalBounds());
}

void SpikeTrap::update(float dt) {
    t_ += dt;
    if (moveDistance_ > 0.f) {
        pos_.setX(startX_ + sin(t_ * speed_) * moveDistance_);
    }
    pos_.setY(startY_);
    shape_.setPosition(pos_.getX(), pos_.getY());
}

void SpikeTrap::draw(sf::RenderTarget& target) const {
    if (!active_) return;

    // little triangles so it reads as spikes not a red bar
    const float w = 56.f;
    const int n = 4;
    const float tooth = w / n;
    for (int i = 0; i < n; ++i) {
        sf::ConvexShape tri;
        tri.setPointCount(3);
        const float x = pos_.getX() + i * tooth;
        const float y = pos_.getY();
        tri.setPoint(0, sf::Vector2f(x, y + 24.f));
        tri.setPoint(1, sf::Vector2f(x + tooth, y + 24.f));
        tri.setPoint(2, sf::Vector2f(x + tooth * 0.5f, y));
        tri.setFillColor(sf::Color(230, 55, 80));
        target.draw(tri);
    }
}

void SpikeTrap::reset() {
    t_ = 0.f;
    pos_.setX(startX_);
    pos_.setY(startY_);
    active_ = true;
    shape_.setPosition(pos_.getX(), pos_.getY());
}