#include "traps/ControlTrap.h"
#include "stubs/Player.h"

ControlTrap::ControlTrap(const Position& pos, float width, float height)
    : Trap(pos, TrapKind::ControlSwap), alreadyHit_(false) {
    shape_.setSize(sf::Vector2f(width, height));
    shape_.setFillColor(sf::Color(170, 90, 255));
    shape_.setPosition(pos_.getX(), pos_.getY());
    tag_ = "ctrl_trap";
}

bool ControlTrap::hurtsPlayer(const Player& player) const {
    (void)player;
    return false;
}

void ControlTrap::onPlayerTouch(Player& player) {
    if (alreadyHit_) return;

    const sf::FloatRect pb = player.getBounds();
    if (pb.intersects(shape_.getGlobalBounds())) {
        player.setControlsFlipped(!player.areControlsFlipped());
        alreadyHit_ = true;
    }
}

void ControlTrap::draw(sf::RenderTarget& target) const {
    if (active_) target.draw(shape_);
}

void ControlTrap::reset() {
    alreadyHit_ = false;
    active_ = true;
}