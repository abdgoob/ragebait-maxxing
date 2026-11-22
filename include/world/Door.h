#pragma once

#include "world/Platform.h"

using namespace std;

class PressureSwitch;

// a platform that stops being solid while its switch is held down
// the level owns the switch, the door only looks at it

class Door : public Platform {
public:
    Door();
    Door(const Position& pos, float width, float height, const PressureSwitch* link);

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;
    bool isSolid() const override;
    void reset() override;

    bool isOpen() const;
    void setLink(const PressureSwitch* link);

private:
    const PressureSwitch* link_;
    bool open_;
};
