#pragma once

#include "traps/Trap.h"
#include "world/Checkpoint.h"
#include "world/Exit.h"
#include "world/PressureSwitch.h"
#include "world/Door.h"
#include "world/MovingPlatform.h"
#include "developer/DeveloperBoss.h"
#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

class Player;
class DeveloperDirector;
class PatchManager;

enum class LevelEvent { None, Checkpoint, FakeCheckpoint, FakeExit, RealExit, BossFakeWin, BossWin };

// bare level shell, mostly here so traps have somewhere to live

class Level {
public:
    static const int MAX_TRAPS = 32;

    Level();
    explicit Level(const string& name);
    ~Level();

    void addTrap(Trap* trap);
    void clearTraps();
    void setName(const string& name);
    void setPit(float left, float right);
    void setupTrolls(int chapter);
    void setAssets(const AssetManager* assets);
    void setInteractionBounds(bool ghostOn, const sf::FloatRect& ghost, bool popupOn, const sf::FloatRect& popup);
    void addPlatform(Platform* platform);
    bool isGhostDoorOpen() const;
    const DeveloperBoss& getBoss() const;
    LevelEvent takeEvent();
    bool hasBetrayed() const;
    void setBaseGravity(float g);
    float getBaseGravity() const;

    void update(float dt, Player& player, DeveloperDirector& dev, PatchManager& patches);
    void draw(sf::RenderTarget& target) const;

    void checkTraps(Player& player, DeveloperDirector& dev, PatchManager& patches);
    void resetTraps();

    const string& getName() const;

private:
    PressureSwitch* plate_ = 0;
    Door* door_ = 0;
    Platform* platforms_[8] = {0};
    int platformCount_ = 0;
    bool ghostOn_ = false;
    bool popupOn_ = false;
    sf::FloatRect ghostBounds_;
    sf::FloatRect popupBounds_;
    DeveloperBoss boss_;
    void landOnSurface(Player& player, const sf::FloatRect& oldBounds, const sf::FloatRect& surface);
    // hussain: level owns these just like it owns the trap pointers
    const AssetManager* assets_ = 0;
    Checkpoint* checkpoint_ = 0;
    Exit* exits_[2] = { 0, 0 };
    int chapter_ = 1;
    bool betrayed_ = false;
    float betrayalTimer_ = 0.f;
    float glitchTimer_ = 0.f;
    LevelEvent event_ = LevelEvent::None;
    Trap* finishSpike_ = 0; // borrowed from traps_, never delete this twice
    void checkWorld(Player& player, DeveloperDirector& dev, PatchManager& patches);
    void clearWorld();
    string name_;
    Trap* traps_[MAX_TRAPS];
    int trapCount_;
    float baseGravity_;
    sf::RectangleShape ground_;
    sf::RectangleShape pit_;
    float pitLeft_;
    float pitRight_;

    void keepOnGround(Player& player);
    void drawBackground(sf::RenderTarget& target) const;

    Level(const Level& other);
    Level& operator=(const Level& other);
};
