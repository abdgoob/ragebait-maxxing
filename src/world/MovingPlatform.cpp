#include "world/MovingPlatform.h"

#include <cmath>

using namespace std;

namespace {
const float kDefaultSpeed = 60.f;
}

MovingPlatform::MovingPlatform()
    : MovingPlatform(Position(0.f, 0.f), 100.f, 20.f, Position(120.f, 0.f), kDefaultSpeed) {}

MovingPlatform::MovingPlatform(const Position& start, float width, float height, const Position& travel,
                               float speed)
    : Platform(start, width, height), start_(start), travel_(travel), speed_(speed), progress_(0.f), dir_(1) {
    tag_ = "moving platform";
    shape_.setFillColor(sf::Color(80, 70, 110));
}

void MovingPlatform::update(float dt) {
    lastMove_ = Position(0.f, 0.f);
    if (!enabled_ || speed_ <= 0.f) return;

    const float length = sqrt(travel_.getX() * travel_.getX() + travel_.getY() * travel_.getY());
    if (length <= 0.f) return;

    // ping pong between the two ends at a steady speed
    progress_ += dir_ * speed_ * dt / length;
    if (progress_ >= 1.f) {
        progress_ = 1.f;
        dir_ = -1;
    } else if (progress_ <= 0.f) {
        progress_ = 0.f;
        dir_ = 1;
    }

    const Position next = start_ + travel_ * progress_;
    lastMove_ = next - pos_;
    pos_ = next;
    syncShape();
}

void MovingPlatform::reset() {
    progress_ = 0.f;
    dir_ = 1;
    pos_ = start_;
    lastMove_ = Position(0.f, 0.f);
    syncShape();
}
