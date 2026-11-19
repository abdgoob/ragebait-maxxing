#pragma once

#include "core/GameObject.h"

using namespace std;

// hussain's player: smooth movement, jump feel, gravity and landing
// still lives in stubs/ so the trap and patch includes dont break

class Player : public GameObject {
public:
    Player();
    explicit Player(const Position& start);
    // GameObject blocks copying so game can do player_ = Player(...) through this
    Player& operator=(const Player& other);

    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;

    void applyGravity(float strength, float dt);
    void handleInput(bool left, bool right, bool jump);
    void landOn(float surfaceY);
    bool isOnGround() const;
    const Position& getVelocity() const;

    sf::FloatRect getBounds() const;

    float getGravityScale() const;
    void setGravityScale(float scale);

    bool areControlsFlipped() const;
    void setControlsFlipped(bool flipped);

    void kill();
    void respawn();
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
    int moveDir_;
    bool jumpHeld_;
    float coyoteTimer_;
    float jumpBufferTimer_;

    void syncBody();
};
