#include "traps/ControlTrap.h"
#include "stubs/Player.h"

using namespace std;

ControlTrap::ControlTrap(const Position& pos, float width, float height)
    : Trap(pos, TrapKind::ControlSwap), alreadyHit_(false) {
    shape_.setSize(sf::Vector2f(width, height));
    shape_.setFillColor(sf::Color(150, 80, 230));
    shape_.setOutlineColor(sf::Color(220, 180, 255));
    shape_.setOutlineThickness(2.f);
    shape_.setPosition(pos_.getX(), pos_.getY());
    tag_ = "ctrl_trap";
}

bool ControlTrap::hurtsPlayer(const Player& player) const {
    (void)player;
    return false;
}

void ControlTrap::onPlayerTouch(Player& player) {
    if (alreadyHit_) return;

    if (player.getBounds().intersects(shape_.getGlobalBounds())) {
        player.setControlsFlipped(!player.areControlsFlipped());
        alreadyHit_ = true;
        shape_.setFillColor(sf::Color(80, 50, 110));
    }
}

void ControlTrap::draw(sf::RenderTarget& target) const {
    if (!active_) return;
    const sf::Color tint = alreadyHit_ ? sf::Color(130, 130, 130) : sf::Color::White;
    if (assets_ && assets_->draw(target, Art::Control, getBounds(), tint)) return;
    if (active_) target.draw(shape_);
}

void ControlTrap::reset() {
    alreadyHit_ = false;
    active_ = true;
    shape_.setFillColor(sf::Color(150, 80, 230));
}
