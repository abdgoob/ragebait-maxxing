#pragma once

#include "traps/Trap.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

class Player;
class DeveloperDirector;
class PatchManager;

// bare level shell, mostly here so traps have somewhere to live

class Level {
public:
    Level();
    explicit Level(const std::string& name);

    void addTrap(std::unique_ptr<Trap> trap);
    void setBaseGravity(float g);
    float getBaseGravity() const;

    void update(float dt, Player& player, DeveloperDirector& dev, PatchManager& patches);
    void draw(sf::RenderTarget& target) const;

    void checkTraps(Player& player, DeveloperDirector& dev, PatchManager& patches);
    void resetTraps();

    const std::string& getName() const;

private:
    std::string name_;
    std::vector<std::unique_ptr<Trap>> traps_;
    float baseGravity_;
    sf::RectangleShape ground_;
};