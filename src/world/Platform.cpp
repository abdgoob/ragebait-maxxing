#include "world/Platform.h"

using namespace std;

Platform::Platform() : Platform(Position(0.f, 0.f), 100.f, 20.f) {}

Platform::Platform(const Position& pos, float width, float height)
    : GameObject(pos, "platform"), w_(width), h_(height), enabled_(true), lastMove_(0.f, 0.f) {
    // same colour as the old ground so nothing looks different yet
    shape_.setFillColor(sf::Color(60, 60, 70));
    syncShape();
}

Platform::Platform(float x, float y, float width, float height)
    : Platform(Position(x, y), width, height) {}

Platform::~Platform() {}

void Platform::update(float dt) {
    (void)dt;
    // plain platforms just sit there
    lastMove_ = Position(0.f, 0.f);
}

void Platform::draw(sf::RenderTarget& target) const {
    if (enabled_) target.draw(shape_);
}

bool Platform::isSolid() const { return enabled_; }

void Platform::reset() { lastMove_ = Position(0.f, 0.f); }

sf::FloatRect Platform::getBounds() const {
    // worked out by hand so outlines never change the hitbox
    return sf::FloatRect(pos_.getX(), pos_.getY(), w_, h_);
}

const Position& Platform::getLastMove() const { return lastMove_; }

void Platform::setSize(float width, float height) {
    w_ = width;
    h_ = height;
    syncShape();
}

void Platform::setRect(float x, float y, float width, float height) {
    pos_ = Position(x, y);
    w_ = width;
    h_ = height;
    syncShape();
}

void Platform::setColor(const sf::Color& color) { shape_.setFillColor(color); }
void Platform::setEnabled(bool on) { enabled_ = on; }
bool Platform::isEnabled() const { return enabled_; }

void Platform::syncShape() {
    shape_.setSize(sf::Vector2f(w_, h_));
    shape_.setPosition(pos_.getX(), pos_.getY());
}
