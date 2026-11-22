#pragma once

#include "core/GameObject.h"

using namespace std;

// solid block, you can stand on it, bump your head on it and walk into its sides
// level keeps these in an array and asks isSolid every frame so patches can switch them off

class Platform : public GameObject {
public:
    Platform();
    Platform(const Position& pos, float width, float height);
    Platform(float x, float y, float width, float height);
    ~Platform() override;

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;
    virtual bool isSolid() const;
    virtual void reset();

    sf::FloatRect getBounds() const;
    // how far it moved this frame, level uses it to carry the player along
    const Position& getLastMove() const;

    void setSize(float width, float height);
    void setRect(float x, float y, float width, float height);
    void setColor(const sf::Color& color);
    void setEnabled(bool on);
    bool isEnabled() const;

protected:
    float w_;
    float h_;
    bool enabled_;
    Position lastMove_;
    sf::RectangleShape shape_;

    void syncShape();
};
