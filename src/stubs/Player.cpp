#include "stubs/Player.h"

using namespace std;

namespace {
const float kWidth = 28.f;
const float kHeight = 36.f;
const float kMaxSpeed = 220.f;
const float kGroundAccel = 1800.f;
const float kGroundFriction = 2200.f;
const float kAirAccel = 1100.f;
const float kJump = 380.f;
const float kJumpCut = 0.45f;
const float kMaxFall = 700.f;
const float kCoyoteTime = 0.1f;
const float kJumpBuffer = 0.12f;
const float kFallDeathY = 520.f;
}

Player::Player()
    : GameObject(Position(80.f, 300.f), "player"),
      velocity_(0.f, 0.f),
      spawn_(80.f, 300.f),
      gravityScale_(1.f),
      controlsFlipped_(false),
      alive_(true),
      deaths_(0),
      onGround_(false),
      moveDir_(0),
      jumpHeld_(false),
      coyoteTimer_(0.f),
      jumpBufferTimer_(0.f) {
    body_.setSize(sf::Vector2f(kWidth, kHeight));
    body_.setFillColor(sf::Color(90, 200, 255));
    syncBody();
}

Player::Player(const Position& start)
    : Player() {
    setPosition(start);
    spawn_ = start;
    syncBody();
}

Player& Player::operator=(const Player& other) {
    if (this == &other) return *this;

    // copy the state but keep our own id
    pos_ = other.pos_;
    tag_ = other.tag_;
    velocity_ = other.velocity_;
    spawn_ = other.spawn_;
    body_ = other.body_;
    gravityScale_ = other.gravityScale_;
    controlsFlipped_ = other.controlsFlipped_;
    alive_ = other.alive_;
    deaths_ = other.deaths_;
    onGround_ = other.onGround_;
    moveDir_ = other.moveDir_;
    jumpHeld_ = other.jumpHeld_;
    coyoteTimer_ = other.coyoteTimer_;
    jumpBufferTimer_ = other.jumpBufferTimer_;
    return *this;
}

void Player::update(float dt) {
    if (!alive_) return;

    // speed up towards the target speed instead of snapping to it
    const float target = moveDir_ * kMaxSpeed;
    float accel = kAirAccel;
    if (onGround_) accel = moveDir_ != 0 ? kGroundAccel : kGroundFriction;

    float vx = velocity_.getX();
    if (vx < target) {
        vx += accel * dt;
        if (vx > target) vx = target;
    } else if (vx > target) {
        vx -= accel * dt;
        if (vx < target) vx = target;
    }
    velocity_.setX(vx);

    // coyote time lets you jump a tiny bit after walking off a ledge
    if (onGround_) coyoteTimer_ = kCoyoteTime;
    else if (coyoteTimer_ > 0.f) coyoteTimer_ -= dt;

    if (jumpBufferTimer_ > 0.f && coyoteTimer_ > 0.f) {
        velocity_.setY(-kJump);
        jumpBufferTimer_ = 0.f;
        coyoteTimer_ = 0.f;
    } else if (jumpBufferTimer_ > 0.f) {
        jumpBufferTimer_ -= dt;
    }

    pos_ += velocity_ * dt;
    // level calls landOn again if we are still standing on something
    onGround_ = false;
    syncBody();

    if (pos_.getY() > kFallDeathY) {
        kill();
    }
}

void Player::draw(sf::RenderTarget& target) const {
    if (!alive_) return;
    if (assets_ && assets_->draw(target, Art::Player, getBounds())) return;
    target.draw(body_);
}

void Player::applyGravity(float strength, float dt) {
    if (!alive_) return;

    float vy = velocity_.getY() + strength * gravityScale_ * dt;
    if (vy > kMaxFall) vy = kMaxFall;
    velocity_.setY(vy);
}

void Player::handleInput(bool left, bool right, bool jump) {
    if (!alive_) return;

    if (controlsFlipped_) {
        const bool tmp = left;
        left = right;
        right = tmp;
    }

    moveDir_ = 0;
    if (left) moveDir_ -= 1;
    if (right) moveDir_ += 1;

    // only a fresh press jumps, holding space doesnt bunny hop
    if (jump && !jumpHeld_) jumpBufferTimer_ = kJumpBuffer;

    // letting go early gives a shorter jump
    if (!jump && jumpHeld_ && velocity_.getY() < 0.f) {
        velocity_.setY(velocity_.getY() * kJumpCut);
    }
    jumpHeld_ = jump;
}

void Player::landOn(float surfaceY) {
    if (!alive_ || velocity_.getY() < 0.f) return;

    pos_.setY(surfaceY - kHeight);
    velocity_.setY(0.f);
    onGround_ = true;
    syncBody();
}

bool Player::isOnGround() const { return onGround_; }
const Position& Player::getVelocity() const { return velocity_; }

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
    onGround_ = false;
}

void Player::respawn() { respawn(spawn_); }

void Player::respawn(const Position& at) {
    setPosition(at);
    spawn_ = at;
    velocity_ = Position(0.f, 0.f);
    alive_ = true;
    // spawn might be in the air, level checks the ground again
    onGround_ = false;
    moveDir_ = 0;
    coyoteTimer_ = 0.f;
    jumpBufferTimer_ = 0.f;
    syncBody();
}

bool Player::isAlive() const { return alive_; }

int Player::getDeathCount() const { return deaths_; }
void Player::addDeath() { ++deaths_; }

const Position& Player::getSpawnPoint() const { return spawn_; }
void Player::setSpawnPoint(const Position& p) { spawn_ = p; }

void Player::syncBody() { body_.setPosition(pos_.getX(), pos_.getY()); }
