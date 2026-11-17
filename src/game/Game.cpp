#include "game/Game.h"
#include "traps/TrapFactory.h"
#include <iostream>

using namespace std;

Game::Game()
    : player_(Position(80.f, 380.f)), level_("arsalan test pit"), state_(GameState::Menu) {
    level_.addTrap(TrapFactory::create(TrapKind::Spike, Position(340.f, 420.f)));
    level_.addTrap(TrapFactory::create(TrapKind::FakePlatform, Position(220.f, 400.f)));
    level_.addTrap(TrapFactory::create(TrapKind::ControlSwap, Position(520.f, 400.f)));
    cout << "menu: Enter starts, M returns here\n";
}

Game::~Game() { patches_.clearAll(player_, level_); }

void Game::start() {
    if (state_ != GameState::Menu) return;

    // new game starts clean, retry below keeps the patches
    patches_.clearAll(player_, level_);
    player_ = Player(Position(80.f, 380.f));
    level_.setBaseGravity(900.f);
    level_.resetTraps();
    dev_ = DeveloperDirector();
    dev_.onLevelStart(level_);
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
    }
}

void Game::draw(sf::RenderTarget& target) const {
    level_.draw(target);
    if (state_ != GameState::Menu) player_.draw(target);
}

GameState Game::getState() const { return state_; }
