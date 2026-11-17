#pragma once

#include "traps/Trap.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

class Player;
class DeveloperDirector;
class PatchManager;

// bare level shell, mostly here so traps have somewhere to live

class Level {
public:
    static const int MAX_TRAPS = 32;

    Level();
    explicit Level(const string& name);
    ~Level();

    void addTrap(Trap* trap);
    void setBaseGravity(float g);
    float getBaseGravity() const;

    void update(float dt, Player& player, DeveloperDirector& dev, PatchManager& patches);
    void draw(sf::RenderTarget& target) const;

    void checkTraps(Player& player, DeveloperDirector& dev, PatchManager& patches);
    void resetTraps();

    const string& getName() const;

private:
    string name_;
    Trap* traps_[MAX_TRAPS];
    int trapCount_;
    float baseGravity_;
    sf::RectangleShape ground_;

    Level(const Level& other);
    Level& operator=(const Level& other);
};