#include "traps/FakePlatform.h"
#include "stubs/Player.h"

using namespace std;

FakePlatform::FakePlatform(const Position& pos, float width, float height)
    : Trap(pos, TrapKind::FakePlatform),
      w_(width),
      h_(height),
      startY_(pos.getY()),
      fallen_(false),
      fallTimer_(0.f) {
    shape_.setSize(sf::Vector2f(w_, h_));
    // same colour as level's ground, no cracks or glowing edge giving the joke away
    shape_.setFillColor(sf::Color(60, 60, 70));
    shape_.setPosition(pos_.getX(), pos_.getY());
}

bool FakePlatform::hurtsPlayer(const Player& player) const {
    (void)player;
    // collapsing is the joke, the pit / spikes below do the killing
    return false;
}

void FakePlatform::onPlayerTouch(Player& player) {
    if (fallen_) return;

    const sf::FloatRect pb = player.getBounds();
    const sf::FloatRect plat = shape_.getGlobalBounds();
    const bool fromAbove = pb.top + pb.height <= plat.top + 12.f;

    if (fromAbove && pb.intersects(plat)) {
        player.landOn(plat.top);
        fallen_ = true;
        fallTimer_ = 0.35f;
    }
}

void FakePlatform::update(float dt) {
    if (fallen_ && fallTimer_ > 0.f) {
        fallTimer_ -= dt;
        pos_.setY(pos_.getY() + 420.f * dt);
        shape_.setPosition(pos_.getX(), pos_.getY());
    }
}

void FakePlatform::draw(sf::RenderTarget& target) const {
    if (fallen_ && fallTimer_ <= 0.f) return;
    // floor lids cover the full visible ground depth; the hitbox stays the same
    sf::RectangleShape floor = shape_;
    if (startY_ == 480.f) floor.setSize(sf::Vector2f(w_, 40.f));
    target.draw(floor);
}

void FakePlatform::reset() {
    fallen_ = false;
    fallTimer_ = 0.f;
    active_ = true;
    pos_.setY(startY_);
    shape_.setPosition(pos_.getX(), pos_.getY());
}

bool FakePlatform::isFallen() const { return fallen_; }

bool FakePlatform::holdsPlayer() const { return !fallen_; }
