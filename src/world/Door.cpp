#include "world/Door.h"
#include "world/PressureSwitch.h"

using namespace std;

Door::Door() : Door(Position(0.f, 0.f), 20.f, 80.f, 0) {}

Door::Door(const Position& pos, float width, float height, const PressureSwitch* link)
    : Platform(pos, width, height), link_(link), open_(false) {
    tag_ = "door";
    shape_.setFillColor(sf::Color(150, 95, 60));
}

void Door::update(float dt) {
    (void)dt;
    lastMove_ = Position(0.f, 0.f);
    // only open while someone is actually standing on the plate
    open_ = link_ != 0 && link_->isPressed();
}

void Door::draw(sf::RenderTarget& target) const {
    if (!enabled_) return;
    if (!open_) {
        target.draw(shape_);
        return;
    }
    // open doors leave a faint outline so you know where it will slam shut
    sf::RectangleShape outline = shape_;
    outline.setFillColor(sf::Color(150, 95, 60, 40));
    outline.setOutlineColor(sf::Color(150, 95, 60, 140));
    outline.setOutlineThickness(1.f);
    target.draw(outline);
}

bool Door::isSolid() const { return enabled_ && !open_; }

void Door::reset() {
    Platform::reset();
    open_ = false;
}

bool Door::isOpen() const { return open_; }
void Door::setLink(const PressureSwitch* link) { link_ = link; }
