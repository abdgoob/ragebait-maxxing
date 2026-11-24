#pragma once
#include <SFML/Graphics.hpp>
class Player;

// arsalan: two timed rounds, one warning lane and one final shutdown switch
class DeveloperBoss {
public:
    DeveloperBoss();
    void reset();
    void update(float dt, Player& player);
    void draw(sf::RenderTarget& target) const;
    bool takeFakeWin();
    bool isReadyForShutdown() const;
    bool isDefeated() const;
    int getPhase() const;
    float getWarningX() const;
    bool areControlsReversed() const;
    bool isControlWarning() const;
private:
    int phase_;
    float roundTime_;
    float attackTime_;
    float warningX_;
    bool fakeWin_;
    bool defeated_;
    float controlTimer_;
    bool controlsReversed_;
};
