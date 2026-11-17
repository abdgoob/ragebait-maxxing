#pragma once

#include "core/GameObject.h"

using namespace std;

// temp player stub so my traps/patches compile, hussan owns the real one

class Player : public GameObject {
public:
    Player();
    explicit Player(const Position& start);

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;

    void applyGravity(float strength, float dt);
    void handleInput(bool left, bool right, bool jump);

    sf::FloatRect getBounds() const;

    float getGravityScale() const;
    void setGravityScale(float scale);

    bool areControlsFlipped() const;
    void setControlsFlipped(bool flipped);

    void kill();
    void respawn(const Position& at);
    bool isAlive() const;

    int getDeathCount() const;
    void addDeath();

    const Position& getSpawnPoint() const;
    void setSpawnPoint(const Position& p);

private:
    Position velocity_;
    Position spawn_;
    sf::RectangleShape body_;
    float gravityScale_;
    bool controlsFlipped_;
    bool alive_;
    int deaths_;
    bool onGround_;
};