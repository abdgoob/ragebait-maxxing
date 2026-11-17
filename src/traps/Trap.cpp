#include "traps/Trap.h"
#include "stubs/Player.h"

Trap::Trap(const Position& pos, TrapKind kind)
    : GameObject(pos, "trap"), kind_(kind), active_(true) {
    shape_.setSize(sf::Vector2f(48.f, 24.f));
    shape_.setFillColor(sf::Color(200, 80, 80));
    shape_.setPosition(pos_.getX(), pos_.getY());
}

Trap::~Trap() = default;

TrapKind Trap::getKind() const { return kind_; }
bool Trap::isActive() const { return active_; }
void Trap::setActive(bool on) { active_ = on; }

void Trap::onPlayerTouch(Player& player) {
    (void)player;
}

void Trap::reset() {}

void Trap::update(float dt) {
    (void)dt;
    shape_.setPosition(pos_.getX(), pos_.getY());
}

void Trap::draw(sf::RenderTarget& target) const {
    if (active_) target.draw(shape_);
}

sf::FloatRect Trap::getBounds() const {
    return shape_.getGlobalBounds();
}