#pragma once

#include "core/GameObject.h"

class Player;

enum class TrapKind {
    Spike,
    FakePlatform,
    ControlSwap
};

// base trap, each type does its own annoying thing

class Trap : public GameObject {
public:
    Trap(const Position& pos, TrapKind kind);
    ~Trap() override;

    TrapKind getKind() const;
    bool isActive() const;
    void setActive(bool on);

    virtual bool hurtsPlayer(const Player& player) const = 0;
    virtual void onPlayerTouch(Player& player);
    virtual void reset();

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;

    sf::FloatRect getBounds() const;

protected:
    TrapKind kind_;
    bool active_;
    sf::RectangleShape shape_;
};