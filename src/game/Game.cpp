#include "game/Game.h"
#include "levels/LevelBuilder.h"
#include <iostream>

using namespace std;

Game::Game()
    : player_(Position(80.f, 444.f)), level_("01 everything is fine"), state_(GameState::Menu), chapter_(1) {
    loadChapter(1);
    cout << "menu: Enter starts, M returns here, walk right to swap chapters for now\n";
}

Game::~Game() { patches_.clearAll(player_, level_); }

void Game::loadChapter(int n) {
    if (n < 1) n = 1;
    if (n > 3) n = 3;
    chapter_ = n;
    patches_.clearAll(player_, level_);
    level_.setBaseGravity(900.f);
    LevelBuilder::fillChapter(level_, chapter_);
    player_.setGravityScale(1.f);
    player_.setControlsFlipped(false);
    player_.setSpawnPoint(Position(80.f, 444.f));
    player_.respawn();
    dev_ = DeveloperDirector();
    dev_.setPatchHappy(chapter_ == 3);
    dev_.onLevelStart(level_);
}

void Game::start() {
    if (state_ != GameState::Menu) return;

    loadChapter(1);
    state_ = GameState::Playing;
    cout << dev_.getLastTaunt() << "\n";
    cout << "arrows move, space jumps, Escape pauses, R retries\n";
}

void Game::pause() {
    if (state_ != GameState::Playing) return;
    state_ = GameState::Paused;
    cout << "paused: Escape resumes\n";
}

void Game::resume() {
    if (state_ != GameState::Paused) return;
    state_ = GameState::Playing;
    cout << "resumed\n";
}

void Game::retry() {
    if (state_ != GameState::Playing && state_ != GameState::Dead) return;

    patches_.applyQueued(player_, level_);
    level_.resetTraps();
    dev_.onRetry(player_);
    state_ = GameState::Playing;
    cout << dev_.getLastTaunt() << "\n";
}

void Game::returnToMenu() {
    if (state_ == GameState::Menu) return;
    state_ = GameState::Menu;
    cout << "menu: Enter starts a fresh game\n";
}

void Game::update(float dt, bool left, bool right, bool jump) {
    if (state_ != GameState::Playing || dt <= 0.f) return;

    player_.handleInput(left, right, jump);
    level_.update(dt, player_, dev_, patches_);
    if (!player_.isAlive()) {
        state_ = GameState::Dead;
        cout << dev_.getLastTaunt() << " | deaths: " << player_.getDeathCount()
             << " | R retries\n";
        if (dev_.getLastPatchReason().size()) {
            cout << dev_.getLastPatchReason() << "\n";
        }
        return;
    }

    // temp until hussain puts a real exit in, walking off the right loads next layout
    if (player_.getPosition().getX() > 910.f) {
        if (chapter_ < 3) {
            loadChapter(chapter_ + 1);
            cout << "ok you made it, next is " << level_.getName() << "\n";
            cout << dev_.getLastTaunt() << "\n";
        } else {
            player_.setPosition(880.f, player_.getPosition().getY());
            cout << "thats all of arsalan layouts for now, hussain still owns the real exit\n";
        }
    }
}

void Game::draw(sf::RenderTarget& target) const {
    level_.draw(target);
    if (state_ != GameState::Menu) player_.draw(target);
}

GameState Game::getState() const { return state_; }