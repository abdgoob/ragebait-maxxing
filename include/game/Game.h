#pragma once
#include "developer/DeveloperDirector.h"
#include "patches/PatchManager.h"
#include "stubs/Level.h"
#include "stubs/Player.h"

using namespace std;

enum class GameState { Menu, Playing, Paused, Dead };

class Game {
public:
    Game();
    ~Game();
    void start();
    void pause();
    void resume();
    void retry();
    void returnToMenu();
    void update(float dt, bool left, bool right, bool jump);
    void draw(sf::RenderTarget& target) const;
    GameState getState() const;
private:
    Player player_;
    Level level_;
    DeveloperDirector dev_;
    PatchManager patches_;
    GameState state_;
    Game(const Game& other);
    Game& operator=(const Game& other);
};
