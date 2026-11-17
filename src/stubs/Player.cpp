#include "stubs/Player.h"

using namespace std;

namespace {
const float kWidth = 28.f;
const float kHeight = 36.f;
const float kMoveSpeed = 200.f;
const float kJump = 380.f;
}

Player::Player()
    : GameObject(Position(80.f, 300.f), "player"),
      velocity_(0.f, 0.f),
      spawn_(80.f, 300.f),
      gravityScale_(1.f),
      controlsFlipped_(false),
      alive_(true),
      deaths_(0),
      onGround_(false) {
    body_.setSize(sf::Vector2f(kWidth, kHeight));
    body_.setFillColor(sf::Color(90, 200, 255));
    body_.setPosition(pos_.getX(), pos_.getY());
}

Player::Player(const Position& start)
    : Player() {
    setPosition(start);
    spawn_ = start;
    body_.setPosition(pos_.getX(), pos_.getY());
}

void Player::update(float dt) {
    if (!alive_) return;

    pos_.setX(pos_.getX() + velocity_.getX() * dt);
    pos_.setY(pos_.getY() + velocity_.getY() * dt);
    body_.setPosition(pos_.getX(), pos_.getY());

    if (pos_.getY() > 520.f) {
        kill();
    }
}

void Player::draw(sf::RenderTarget& target) const {
    if (alive_) target.draw(body_);
}

void Player::applyGravity(float strength, float dt) {
    if (!alive_) return;
    velocity_.setY(velocity_.getY() + strength * gravityScale_ * dt);

    if (pos_.getY() >= 440.f) {
        pos_.setY(440.f);
        velocity_.setY(0.f);
        onGround_ = true;
    } else {
        onGround_ = false;
    }
}

void Player::handleInput(bool left, bool right, bool jump) {
    if (!alive_) return;

    if (controlsFlipped_) {
        const bool tmp = left;
        left = right;
        right = tmp;
    }

    velocity_.setX(0.f);
    if (left) velocity_.setX(-kMoveSpeed);
    if (right) velocity_.setX(kMoveSpeed);

    if (jump && onGround_) {
        velocity_.setY(-kJump);
        onGround_ = false;
    }
}

sf::FloatRect Player::getBounds() const {
    return sf::FloatRect(pos_.getX(), pos_.getY(), kWidth, kHeight);
}

float Player::getGravityScale() const { return gravityScale_; }
void Player::setGravityScale(float scale) { gravityScale_ = scale; }

bool Player::areControlsFlipped() const { return controlsFlipped_; }
void Player::setControlsFlipped(bool flipped) { controlsFlipped_ = flipped; }

void Player::kill() {
    alive_ = false;
    ++deaths_;
    velocity_ = Position(0.f, 0.f);
}

void Player::respawn(const Position& at) {
    setPosition(at);
    spawn_ = at;
    velocity_ = Position(0.f, 0.f);
    alive_ = true;
    body_.setPosition(pos_.getX(), pos_.getY());
}

bool Player::isAlive() const { return alive_; }

int Player::getDeathCount() const { return deaths_; }
void Player::addDeath() { ++deaths_; }

const Position& Player::getSpawnPoint() const { return spawn_; }
void Player::setSpawnPoint(const Position& p) { spawn_ = p; }