#include "world/PressureSwitch.h"
#include "stubs/Player.h"

using namespace std;

namespace {
const float kPlateHeight = 8.f;
const float kDefaultWidth = 48.f;
const float kPressedSink = 4.f;
}

PressureSwitch::PressureSwitch() : PressureSwitch(Position(0.f, 0.f), kDefaultWidth) {}

PressureSwitch::PressureSwitch(const Position& pos) : PressureSwitch(pos, kDefaultWidth) {}

PressureSwitch::PressureSwitch(const Position& pos, float width)
    : GameObject(pos, "pressure switch"), w_(width), pressed_(false) {
    plate_.setSize(sf::Vector2f(w_, kPlateHeight));
    plate_.setOutlineColor(sf::Color(30, 30, 40));
    plate_.setOutlineThickness(1.f);
    update(0.f);
}

void PressureSwitch::update(float dt) {
    (void)dt;
    // pressed plates sink a bit and go green so you can see the ghost doing its job
    if (pressed_) {
        plate_.setFillColor(sf::Color(90, 220, 140));
        plate_.setPosition(pos_.getX(), pos_.getY() + kPressedSink);
    } else {
        plate_.setFillColor(sf::Color(200, 170, 70));
        plate_.setPosition(pos_.getX(), pos_.getY());
    }
}

void PressureSwitch::draw(sf::RenderTarget& target) const { target.draw(plate_); }

bool PressureSwitch::touches(const sf::FloatRect& box) const { return getBounds().intersects(box); }

bool PressureSwitch::touches(const Player& player) const { return touches(player.getBounds()); }

void PressureSwitch::setPressed(bool pressed) {
    pressed_ = pressed;
    update(0.f);
}

bool PressureSwitch::isPressed() const { return pressed_; }

void PressureSwitch::reset() { setPressed(false); }

sf::FloatRect PressureSwitch::getBounds() const {
    return sf::FloatRect(pos_.getX(), pos_.getY(), w_, kPlateHeight);
}
