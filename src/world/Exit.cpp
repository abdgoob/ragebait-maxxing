#include "world/Exit.h"
#include "stubs/Player.h"

#include <cmath>

using namespace std;

Exit::Exit() : Exit(Position(0.f, 0.f), 48.f, 64.f, false) {}

Exit::Exit(const Position& pos, float width, float height) : Exit(pos, width, height, false) {}

Exit::Exit(const Position& pos, float width, float height, bool fake)
    : GameObject(pos, fake ? "fake exit" : "exit"),
      w_(width),
      h_(height),
      fake_(fake),
      used_(false),
      enabled_(true),
      pulse_(0.f) {
    frame_.setSize(sf::Vector2f(w_, h_));
    frame_.setPosition(pos_.getX(), pos_.getY());
    frame_.setFillColor(sf::Color(40, 150, 80));
    door_.setSize(sf::Vector2f(w_ - 12.f, h_ - 10.f));
    door_.setPosition(pos_.getX() + 6.f, pos_.getY() + 10.f);
    update(0.f);
}

void Exit::update(float dt) {
    pulse_ += dt;
    // same glow for real and fake so you cant tell them apart
    const int shine = 160 + static_cast<int>(60.f * (0.5f + 0.5f * sin(pulse_ * 3.f)));
    door_.setFillColor(sf::Color(90, shine + 30, 120));
}

void Exit::draw(sf::RenderTarget& target) const {
    if (!enabled_ || used_) return;
    if (assets_ && assets_->draw(target, Art::Exit, getBounds())) return;
    target.draw(frame_);
    target.draw(door_);
}

bool Exit::isReachedBy(const sf::FloatRect& box) const {
    if (!enabled_ || used_) return false;
    return getBounds().intersects(box);
}

bool Exit::isReachedBy(const Player& player) const { return isReachedBy(player.getBounds()); }

bool Exit::isFake() const { return fake_; }

void Exit::use() {
    // the real one stays put, the fake one has done its job
    if (fake_) used_ = true;
}

void Exit::reset() { used_ = false; }

void Exit::setEnabled(bool on) { enabled_ = on; }
bool Exit::isEnabled() const { return enabled_; }

sf::FloatRect Exit::getBounds() const { return sf::FloatRect(pos_.getX(), pos_.getY(), w_, h_); }
