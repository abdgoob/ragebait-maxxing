#pragma once

#include "developer/DeveloperDirector.h"
#include "patches/PatchManager.h"
#include "stubs/Level.h"
#include "stubs/Player.h"
#include "audio/SoundSystem.h"
#include "game/AttemptRecorder.h"
#include "game/ProgressSave.h"
#include "game/GhostReplay.h"
#include "game/RunSummary.h"
#include "ui/GameUI.h"
#include "ui/PopupWindow.h"

using namespace std;

enum class GameState {
    Menu,
    Playing,
    Paused,
    Dead,
    ChapterClear,
    FakeVictory,
    Victory,
    Credits
};

enum class MenuPage {
    Main,
    LevelSelect,
    Settings
};

// abdullah flow and screens, world stuff stays in level and player

class Game {
public:
    Game();
    explicit Game(const string& assetFolder);
    Game(const string& assetFolder, const string& savePath);
    ~Game();

    void start();
    bool startChapter(int chapter);
    void pause();
    void resume();
    void retry();
    void returnToMenu();
    void continueChapter();
    void completeChapter();
    void showFakeVictory();
    void win();

    void handleKey(sf::Keyboard::Key key);
    void mouseMoved(const sf::Vector2f& mouse);
    void mousePressed(const sf::Vector2f& mouse);
    void mouseReleased();
    void lostFocus();
    bool wantsQuit() const;
    bool isReady() const;
    int getCurrentChapter() const;
    int getHighestUnlockedChapter() const;
    MenuPage getMenuPage() const;
    bool isSoundMuted() const;
    const RunSummary& getRunSummary() const;

    // hooks for hussain's ghost switches and chapter 5 platforms
    void setGhostReplayEnabled(bool on);
    void setInterfacePopupEnabled(bool on);
    void enableGhostReplay();
    void disableGhostReplay();
    void enableInterfacePopup();
    void disableInterfacePopup();
    bool hasGhost() const;
    sf::FloatRect getGhostBounds() const;
    bool hasPopupPlatform() const;
    sf::FloatRect getPopupPlatformBounds() const;

    void update(float dt, bool left, bool right, bool jump);
    void draw(sf::RenderTarget& target) const;
    GameState getState() const;

private:
    Player player_;
    Level level_;
    DeveloperDirector dev_;
    PatchManager patches_;
    GameState state_;
    int chapter_;
    GameUI ui_;
    SoundSystem sounds_;
    AttemptRecorder attempts_;
    GhostReplay ghost_;
    PopupWindow popup_;
    ProgressSave progress_;
    RunSummary summary_;
    string savePath_;
    bool quit_;
    bool finalVictory_;
    bool ghostReplayEnabled_;
    bool interfacePopupEnabled_;
    int shownPatch_;
    float messageTimer_;
    float patchPopupTimer_;
    float stateTimer_;
    string message_;
    string patchMessage_;
    string patchVersion_;
    MenuPage menuPage_;

    void loadChapter(int n);
    void saveProgress();
    void toggleSound();
    void showMenuPage(MenuPage page);
    void handleMenuKey(sf::Keyboard::Key key);
    void handleMenuClick(const sf::Vector2f& mouse);
    void showMessage(const string& words);
    void checkPatchMessage();
    void drawHud(sf::RenderTarget& target) const;
    void drawScreen(sf::RenderTarget& target) const;
    void drawMenu(sf::RenderTarget& target) const;

    Game(const Game& other);
    Game& operator=(const Game& other);
};


