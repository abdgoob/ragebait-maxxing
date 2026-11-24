#include "developer/DeveloperBoss.h"
#include "stubs/Player.h"

namespace {
void bossBox(sf::RenderTarget& target, float x, float y, float w, float h, sf::Color color) {
    sf::RectangleShape shape(sf::Vector2f(w, h));
    shape.setPosition(x, y);
    shape.setFillColor(color);
    target.draw(shape);
}
}

DeveloperBoss::DeveloperBoss() { reset(); }
void DeveloperBoss::reset() {
    phase_ = 0;
    roundTime_ = 0.f;
    attackTime_ = 0.f;
    warningX_ = 80.f;
    fakeWin_ = false;
    defeated_ = false;
    controlTimer_ = 0.f;
    controlsReversed_ = false;
}
void DeveloperBoss::update(float dt, Player& player) {
    if (defeated_ || !player.isAlive() || dt <= 0.f) return;
    if (phase_ < 2) {
        roundTime_ += dt;
        // separate from the saved patch state: retry and shutdown always remove this
        controlTimer_ += dt;
        const float flipEvery = phase_ == 0 ? 2.8f : 1.8f;
        if (controlTimer_ >= flipEvery) {
            controlTimer_ -= flipEvery;
            controlsReversed_ = !controlsReversed_;
        }
        // lock on once, then give the player time to leave that lane
        if (attackTime_ == 0.f) warningX_ = player.getPosition().getX() - 16.f;
        attackTime_ += dt;
        const float warning = phase_ == 0 ? 0.85f : 0.62f;
        if (attackTime_ >= warning && attackTime_ < warning + 0.26f
            && player.getBounds().intersects(sf::FloatRect(warningX_, 270, 76, 210))) player.kill();
        if (attackTime_ >= warning + 0.45f) attackTime_ = 0.f;
        if (roundTime_ >= (phase_ == 0 ? 8.f : 12.f)) {
            ++phase_;
            roundTime_ = 0.f;
            attackTime_ = 0.f;
            controlTimer_ = 0.f;
            if (phase_ == 2) controlsReversed_ = false;
            if (phase_ == 1) fakeWin_ = true;
        }
        return;
    }
    if (player.getBounds().intersects(sf::FloatRect(850, 428, 40, 52))) defeated_ = true;
}
bool DeveloperBoss::takeFakeWin() {
    const bool result = fakeWin_;
    fakeWin_ = false;
    return result;
}
bool DeveloperBoss::isReadyForShutdown() const { return phase_ == 2; }
bool DeveloperBoss::isDefeated() const { return defeated_; }
int DeveloperBoss::getPhase() const { return phase_; }
float DeveloperBoss::getWarningX() const { return warningX_; }
bool DeveloperBoss::areControlsReversed() const { return controlsReversed_; }
bool DeveloperBoss::isControlWarning() const {
    return phase_ < 2 && controlTimer_ >= (phase_ == 0 ? 2.8f : 1.8f) - 0.45f;
}
void DeveloperBoss::draw(sf::RenderTarget& target) const {
    const sf::Color face = defeated_ ? sf::Color(100, 230, 180) : sf::Color(244, 124, 144);
    bossBox(target, 390, 174, 180, 105, sf::Color(66, 77, 105));
    bossBox(target, 399, 183, 162, 87, sf::Color(11, 20, 36));
    bossBox(target, 431, 209, 20, 8, face);
    bossBox(target, 509, 209, 20, 8, face);
    bossBox(target, 457, 244, 46, 5, face);
    bossBox(target, 468, 279, 24, 18, sf::Color(66, 77, 105));
    if (phase_ < 2) {
        const float warning = phase_ == 0 ? 0.85f : 0.62f;
        const bool firing = attackTime_ >= warning && attackTime_ < warning + 0.26f;
        bossBox(target, warningX_, 295, 76, 185,
            firing ? sf::Color(255, 100, 125, 220) : sf::Color(255, 170, 100, 45));
        bossBox(target, warningX_, 477, 76, 3, sf::Color(255, 170, 100));
        bossBox(target, 390, 291, 180, 5, sf::Color(20, 30, 46));
        bossBox(target, 390, 291, 180.f * roundTime_ / (phase_ == 0 ? 8.f : 12.f), 5, face);
    }
    bossBox(target, 850, 428, 40, 52, phase_ == 2 ? sf::Color(70, 220, 160) : sf::Color(70, 60, 80));
    bossBox(target, 860, 438, 20, 20, sf::Color(20, 30, 46));
}
