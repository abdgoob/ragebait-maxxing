#include "traps/FakePlatform.h"
#include "stubs/Player.h"

using namespace std;

FakePlatform::FakePlatform(const Position& pos, float width, float height)
    : Trap(pos, TrapKind::FakePlatform),
      w_(width),
      h_(height),
      fallen_(false),
      fallTimer_(0.f) {
    shape_.setSize(sf::Vector2f(w_, h_));
    shape_.setFillColor(sf::Color(140, 200, 120));
    shape_.setPosition(pos_.getX(), pos_.getY());
}

bool FakePlatform::hurtsPlayer(const Player& player) const {
    (void)player;
    return fallen_;
}

void FakePlatform::onPlayerTouch(Player& player) {
    (void)player;
    if (fallen_) return;

    const sf::FloatRect pb = player.getBounds();
    const sf::FloatRect plat = shape_.getGlobalBounds();

    const bool standingOn =
        pb.top + pb.height <= plat.top + 8.f && pb.intersects(plat);

    if (standingOn) {
        fallen_ = true;
        active_ = false;
        fallTimer_ = 0.4f;
    }
}

void FakePlatform::update(float dt) {
    if (fallen_ && fallTimer_ > 0.f) {
        fallTimer_ -= dt;
        pos_.setY(pos_.getY() + 400.f * dt);
        shape_.setPosition(pos_.getX(), pos_.getY());
    }
    Trap::update(dt);
}

void FakePlatform::draw(sf::RenderTarget& target) const {
    if (!fallen_) target.draw(shape_);
}

void FakePlatform::reset() {
    fallen_ = false;
    fallTimer_ = 0.f;
    active_ = true;
    shape_.setPosition(pos_.getX(), pos_.getY());
}

bool FakePlatform::isFallen() const { return fallen_; }