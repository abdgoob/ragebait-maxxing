#include "world/Checkpoint.h"

#include <cmath>

using namespace std;

namespace {
const float kWidth = 32.f;
const float kHeight = 56.f;
}

Checkpoint::Checkpoint() : Checkpoint(Position(0.f, 0.f), false) {}

Checkpoint::Checkpoint(const Position& pos) : Checkpoint(pos, false) {}

Checkpoint::Checkpoint(const Position& pos, bool fake)
    : GameObject(pos, fake ? "fake checkpoint" : "checkpoint"), fake_(fake), reached_(false), glow_(0.f) {
    pole_.setSize(sf::Vector2f(4.f, kHeight));
    pole_.setFillColor(sf::Color(190, 190, 200));
    pole_.setPosition(pos_.getX() + 2.f, pos_.getY());
    flag_.setSize(sf::Vector2f(24.f, 16.f));
    flag_.setPosition(pos_.getX() + 6.f, pos_.getY() + 2.f);
    update(0.f);
}

void Checkpoint::update(float dt) {
    glow_ += dt;
    if (fake_) {
        // the fake one is the prettiest one on purpose
        const int shine = 200 + static_cast<int>(55.f * (0.5f + 0.5f * sin(glow_ * 5.f)));
        flag_.setFillColor(sf::Color(255, shine, 90));
    } else if (reached_) {
        flag_.setFillColor(sf::Color(90, 220, 140));
    } else {
        flag_.setFillColor(sf::Color(120, 120, 135));
    }
}

void Checkpoint::draw(sf::RenderTarget& target) const {
    if (assets_ && assets_->draw(target, Art::Checkpoint, getBounds(),
        reached_ ? sf::Color(140, 255, 180) : sf::Color::White)) return;
    target.draw(pole_);
    target.draw(flag_);
}

bool Checkpoint::touches(const sf::FloatRect& box) const { return getBounds().intersects(box); }

bool Checkpoint::isFake() const { return fake_; }
bool Checkpoint::isReached() const { return reached_; }
void Checkpoint::reset() { reached_ = false; glow_ = 0.f; update(0.f); }

void Checkpoint::markReached() {
    reached_ = true;
    update(0.f);
}

float Checkpoint::getFloorY() const { return pos_.getY() + kHeight; }

sf::FloatRect Checkpoint::getBounds() const {
    return sf::FloatRect(pos_.getX(), pos_.getY(), kWidth, kHeight);
}
