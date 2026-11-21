#include "game/Game.h"
#include "levels/LevelBuilder.h"
#include <iostream>

using namespace std;

Game::Game() : Game("assets/", "ragebait_save.txt") {}

Game::Game(const string& assetFolder) : Game(assetFolder, "ragebait_save.txt") {}

Game::Game(const string& assetFolder, const string& savePath)
    : player_(Position(80.f, 444.f)), level_("01 everything is fine"),
      state_(GameState::Menu), chapter_(1), ui_(assetFolder), savePath_(savePath), quit_(false),
      finalVictory_(false), ghostReplayEnabled_(false), interfacePopupEnabled_(false),
      chapterStartDeaths_(0), shownPatch_(0), messageTimer_(0.f),
      patchPopupTimer_(0.f), stateTimer_(0.f), runTime_(0.f), menuPage_(MenuPage::Main) {
    progress_.load(savePath_);
    sounds_.setMuted(progress_.isMuted());
    loadChapter(1);
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
    chapterStartDeaths_ = player_.getDeathCount();
    shownPatch_ = patches_.getAnnouncementCount();
    patchPopupTimer_ = 0.f;
    stateTimer_ = 0.f;
    attempts_.clear();
    attempts_.startAttempt(player_.getPosition());
    ghostReplayEnabled_ = false;
    interfacePopupEnabled_ = false;
    popup_.reset();
    showMessage(dev_.getLastTaunt());
}

void Game::showMessage(const string& words) {
    message_ = words;
    messageTimer_ = 5.f;
    cout << words << "\n";
}

void Game::start() {
    if (state_ != GameState::Menu) return;
    int chapter = progress_.getHighestUnlocked();
    if (chapter > 3) chapter = 3;
    startChapter(chapter);
}

bool Game::startChapter(int chapter) {
    if (state_ != GameState::Menu) return false;
    if (chapter < 1 || chapter > 3 || !progress_.isUnlocked(chapter)) return false;
    patches_.clearAll(player_, level_);
    // a new run really is fresh, retries still keep the deaths
    player_ = Player(Position(80.f, 444.f));
    runTime_ = 0.f;
    finalVictory_ = false;
    loadChapter(chapter);
    state_ = GameState::Playing;
    sounds_.play(SoundCue::Click);
    return true;
}

void Game::pause() {
    if (state_ != GameState::Playing) return;
    state_ = GameState::Paused;
    popup_.release();
    sounds_.stopAll();
}

void Game::resume() {
    if (state_ != GameState::Paused) return;
    state_ = GameState::Playing;
    sounds_.play(SoundCue::Click);
}

void Game::retry() {
    if (state_ != GameState::Playing && state_ != GameState::Dead) return;
    // manual retry also counts as an attempt for the future ghost chapter
    if (state_ == GameState::Playing) attempts_.finishAttempt();
    patches_.applyQueued(player_, level_);
    level_.resetTraps();
    dev_.onRetry(player_);
    attempts_.startAttempt(player_.getPosition());
    popup_.reset();
    state_ = GameState::Playing;
    progress_.addRetry();
    saveProgress();
    showMessage(dev_.getLastTaunt());
    sounds_.play(SoundCue::Click);
}

void Game::returnToMenu() {
    if (state_ == GameState::Menu) return;
    state_ = GameState::Menu;
    menuPage_ = MenuPage::Main;
    popup_.release();
    sounds_.stopAll();
}

void Game::completeChapter() {
    if (state_ != GameState::Playing || !player_.isAlive()) return;
    state_ = GameState::ChapterClear;
    stateTimer_ = 0.f;
    popup_.release();
    sounds_.play(chapter_ == 1 ? SoundCue::FakeWin : SoundCue::Chapter);
}

void Game::continueChapter() {
    if (state_ != GameState::ChapterClear) return;
    const int deaths = player_.getDeathCount() - chapterStartDeaths_;
    progress_.completeChapter(chapter_, deaths);
    saveProgress();
    if (chapter_ < 3) {
        loadChapter(chapter_ + 1);
        state_ = GameState::Playing;
        sounds_.play(SoundCue::Chapter);
    } else {
        // these are the three layouts in this build, the boss calls win later
        finalVictory_ = false;
        state_ = GameState::Victory;
        sounds_.play(SoundCue::Victory);
    }
}

void Game::win() {
    if (state_ != GameState::Playing) return;
    finalVictory_ = true;
    state_ = GameState::Victory;
    popup_.release();
    sounds_.play(SoundCue::Victory);
}

void Game::handleKey(sf::Keyboard::Key key) {
    if (state_ == GameState::Menu) {
        handleMenuKey(key);
        return;
    }
    if (key == sf::Keyboard::S) {
        toggleSound();
        return;
    }
    if (key == sf::Keyboard::Q) { quit_ = true; return; }
    if (key == sf::Keyboard::M) { returnToMenu(); return; }
    if (key == sf::Keyboard::R) { retry(); return; }
    if (key == sf::Keyboard::Escape) {
        if (state_ == GameState::Paused) resume();
        else if (state_ == GameState::Credits) state_ = GameState::Victory;
        else pause();
        return;
    }
    if (key != sf::Keyboard::Enter) return;
    if (state_ == GameState::Menu) start();
    else if (state_ == GameState::Paused) resume();
    else if (state_ == GameState::Dead) retry();
    else if (state_ == GameState::ChapterClear) continueChapter();
    else if (state_ == GameState::Victory) state_ = GameState::Credits;
    else if (state_ == GameState::Credits) returnToMenu();
}

void Game::mouseMoved(const sf::Vector2f& mouse) {
    ui_.setMouse(mouse);
    if (state_ == GameState::Playing && interfacePopupEnabled_) popup_.move(mouse);
}

void Game::mousePressed(const sf::Vector2f& mouse) {
    if (state_ == GameState::Menu) {
        handleMenuClick(mouse);
        return;
    }
    if (state_ == GameState::Playing && interfacePopupEnabled_) {
        if (popup_.press(mouse)) sounds_.play(SoundCue::Click);
        return;
    }
    if (state_ == GameState::Playing) return;
    if (sf::FloatRect(290.f, 330.f, 380.f, 48.f).contains(mouse)) {
        handleKey(sf::Keyboard::Enter);
    } else if (sf::FloatRect(290.f, 390.f, 380.f, 48.f).contains(mouse)) {
        if (state_ == GameState::Credits) state_ = GameState::Victory;
        else returnToMenu();
    }
}

void Game::mouseReleased() { popup_.release(); }
void Game::lostFocus() { mouseReleased(); pause(); }
bool Game::wantsQuit() const { return quit_; }
bool Game::isReady() const { return ui_.isReady(); }
int Game::getCurrentChapter() const { return chapter_; }
int Game::getHighestUnlockedChapter() const { return progress_.getHighestUnlocked(); }
MenuPage Game::getMenuPage() const { return menuPage_; }
bool Game::isSoundMuted() const { return sounds_.isMuted(); }

void Game::saveProgress() { progress_.save(savePath_); }

void Game::toggleSound() {
    sounds_.toggleMute();
    progress_.setMuted(sounds_.isMuted());
    saveProgress();
}

void Game::showMenuPage(MenuPage page) {
    menuPage_ = page;
    sounds_.play(SoundCue::Click);
}

void Game::handleMenuKey(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::Q) { quit_ = true; return; }
    if (key == sf::Keyboard::Escape) { showMenuPage(MenuPage::Main); return; }
    if (menuPage_ == MenuPage::Main) {
        if (key == sf::Keyboard::Enter) start();
        else if (key == sf::Keyboard::L) showMenuPage(MenuPage::LevelSelect);
        else if (key == sf::Keyboard::O) showMenuPage(MenuPage::Settings);
        return;
    }
    if (menuPage_ == MenuPage::Settings) {
        if (key == sf::Keyboard::S || key == sf::Keyboard::Enter) toggleSound();
        return;
    }
    int chapter = 0;
    if (key == sf::Keyboard::Num1 || key == sf::Keyboard::Numpad1) chapter = 1;
    else if (key == sf::Keyboard::Num2 || key == sf::Keyboard::Numpad2) chapter = 2;
    else if (key == sf::Keyboard::Num3 || key == sf::Keyboard::Numpad3) chapter = 3;
    else if (key == sf::Keyboard::Num4 || key == sf::Keyboard::Numpad4) chapter = 4;
    else if (key == sf::Keyboard::Num5 || key == sf::Keyboard::Numpad5) chapter = 5;
    else if (key == sf::Keyboard::Num6 || key == sf::Keyboard::Numpad6) chapter = 6;
    if (chapter > 0) {
        if (!startChapter(chapter)) sounds_.play(SoundCue::Death);
    }
}

void Game::handleMenuClick(const sf::Vector2f& mouse) {
    if (menuPage_ == MenuPage::Main) {
        if (sf::FloatRect(290.f, 235.f, 380.f, 46.f).contains(mouse)) start();
        else if (sf::FloatRect(290.f, 293.f, 380.f, 46.f).contains(mouse)) showMenuPage(MenuPage::LevelSelect);
        else if (sf::FloatRect(290.f, 351.f, 380.f, 46.f).contains(mouse)) showMenuPage(MenuPage::Settings);
        else if (sf::FloatRect(290.f, 409.f, 380.f, 46.f).contains(mouse)) quit_ = true;
        return;
    }
    if (menuPage_ == MenuPage::Settings) {
        if (sf::FloatRect(290.f, 285.f, 380.f, 48.f).contains(mouse)) toggleSound();
        else if (sf::FloatRect(290.f, 355.f, 380.f, 48.f).contains(mouse)) showMenuPage(MenuPage::Main);
        return;
    }
    for (int i = 0; i < 6; ++i) {
        const float x = i % 2 == 0 ? 190.f : 490.f;
        const float y = 220.f + (i / 2) * 62.f;
        if (!sf::FloatRect(x, y, 280.f, 46.f).contains(mouse)) continue;
        if (!startChapter(i + 1)) sounds_.play(SoundCue::Death);
        return;
    }
    if (sf::FloatRect(290.f, 420.f, 380.f, 44.f).contains(mouse)) showMenuPage(MenuPage::Main);
}

void Game::setGhostReplayEnabled(bool on) { ghostReplayEnabled_ = on; }
void Game::setInterfacePopupEnabled(bool on) {
    interfacePopupEnabled_ = on;
    popup_.reset();
}
bool Game::hasGhost() const { return ghostReplayEnabled_ && attempts_.hasGhost(); }
sf::FloatRect Game::getGhostBounds() const {
    const Position& p = attempts_.getGhostPosition();
    return sf::FloatRect(p.getX(), p.getY(), 28.f, 36.f);
}
bool Game::hasPopupPlatform() const { return interfacePopupEnabled_ && popup_.isPlatform(); }
sf::FloatRect Game::getPopupPlatformBounds() const { return popup_.getPlatformBounds(); }

void Game::checkPatchMessage() {
    const int count = patches_.getAnnouncementCount();
    if (count == shownPatch_) return;
    shownPatch_ = count;
    patchMessage_ = patches_.getLastAnnouncement();
    patchVersion_ = patches_.getLastVersion();
    patchPopupTimer_ = 5.f;
    sounds_.play(SoundCue::Patch);
}

void Game::update(float dt, bool left, bool right, bool jump) {
    if (dt <= 0.f) return;
    if (dt > 0.05f) dt = 0.05f;
    if (state_ == GameState::ChapterClear) { stateTimer_ += dt; return; }
    if (state_ != GameState::Playing) return;

    runTime_ += dt;
    if (messageTimer_ > 0.f) messageTimer_ -= dt;
    if (patchPopupTimer_ > 0.f) patchPopupTimer_ -= dt;
    attempts_.updateReplay(dt);
    const float oldYVelocity = player_.getVelocity().getY();
    player_.handleInput(left, right, jump);
    level_.update(dt, player_, dev_, patches_);
    attempts_.record(dt, player_.getPosition());
    checkPatchMessage();
    if (!player_.isAlive()) {
        progress_.addDeath();
        saveProgress();
        attempts_.finishAttempt();
        state_ = GameState::Dead;
        showMessage(dev_.getLastTaunt());
        sounds_.play(SoundCue::Death);
        return;
    }
    if (oldYVelocity >= 0.f && player_.getVelocity().getY() < 0.f) sounds_.play(SoundCue::Jump);

    // temp until hussain puts a real exit in, his exit can call completeChapter
    if (player_.getPosition().getX() > 910.f) completeChapter();
}

void Game::drawHud(sf::RenderTarget& target) const {
    ui_.box(target, 0.f, 0.f, 960.f, 60.f, sf::Color(19, 16, 29, 240));
    ui_.text(target, level_.getName(), 24.f, 12.f, 20, sf::Color(210, 185, 255));
    ui_.text(target, "deaths: " + to_string(player_.getDeathCount()), 630.f, 15.f, 18);
    ui_.text(target, sounds_.isMuted() ? "s: sound off" : "s: sound on", 790.f, 18.f, 13);
    string rules = player_.areControlsFlipped() ? "controls: reversed" : "controls: normal";
    rules += "   gravity: " + to_string(static_cast<int>(level_.getBaseGravity()));
    if (patches_.getActiveCount() > 0) {
        const int seconds = static_cast<int>(patches_.getRemainingTime() + 0.999f);
        rules += "   patches: " + to_string(patches_.getActiveCount()) + " / " + to_string(seconds) + "s";
    }
    ui_.text(target, rules, 24.f, 70.f, 14, sf::Color(190, 185, 205));
    if (messageTimer_ > 0.f || state_ == GameState::Dead) {
        ui_.box(target, 24.f, 103.f, 912.f, 64.f, sf::Color(38, 29, 53, 235));
        ui_.monitor(target, 36.f, 114.f, dev_.getMood() >= 4);
        ui_.text(target, "dev: " + message_, 100.f, 122.f, 15);
    }
    if (patchPopupTimer_ > 0.f || (state_ == GameState::Dead && patches_.getActiveCount() > 0)) {
        const float slide = patchPopupTimer_ > 4.8f ? (patchPopupTimer_ - 4.8f) * 100.f : 0.f;
        ui_.box(target, 24.f, 181.f + slide, 912.f, 68.f, sf::Color(84, 43, 111, 245));
        ui_.text(target, "update installed / " + patchVersion_, 38.f, 189.f + slide, 16,
                 sf::Color(240, 190, 255));
        ui_.text(target, patchMessage_, 38.f, 218.f + slide, 14);
    }
    ui_.text(target, "arrows move   space jump   esc pause   r retry   m menu", 24.f, 520.f, 13,
             sf::Color(175, 165, 195));
}

void Game::drawScreen(sf::RenderTarget& target) const {
    ui_.shade(target);
    if (state_ == GameState::Menu) {
        drawMenu(target);
        return;
    }
    ui_.monitor(target, 456.f, 52.f, state_ == GameState::Dead);
    string title;
    string line;
    string detail;
    string first;
    string second = "m / back to menu";
    if (state_ == GameState::Paused) {
        title = "taking a break?";
        line = "fine. the traps can wait.";
        detail = "game and patch timers are paused";
        first = "enter / keep going";
    } else if (state_ == GameState::Dead) {
        title = "you died. again.";
        line = dev_.getLastTaunt();
        detail = "deaths: " + to_string(player_.getDeathCount());
        if (patches_.getActiveCount() > 0) detail += " / update installed: " + patches_.getLastVersion();
        first = "r or enter / try again";
    } else if (state_ == GameState::ChapterClear) {
        title = chapter_ == 1 && stateTimer_ < 1.5f ? "congratulations!" : "player review";
        line = chapter_ == 1 ? "you finished the tutorial. thats it." : "chapter cleared. suspicious.";
        const int deaths = player_.getDeathCount() - chapterStartDeaths_;
        detail = "chapter deaths: " + to_string(deaths) + " / rating: ";
        detail += deaths == 0 ? "probably luck" : (deaths < 5 ? "concerning" : "very committed");
        first = "enter / continue suffering";
    } else if (state_ == GameState::Victory) {
        title = finalVictory_ ? "the final hotfix" : "current build cleared";
        line = finalVictory_ ? "you reached the switch. dev has disconnected." : "all 3 chapters done. dev is making more.";
        detail = "total deaths: " + to_string(player_.getDeathCount()) + " / time: "
                 + to_string(static_cast<int>(runTime_)) + "s";
        first = "enter / credits";
    } else if (state_ == GameState::Credits) {
        title = "blame these people";
        line = "hussain / movement and physical world";
        detail = "arsalan / traps patches and level design";
        first = "enter / back to menu";
        second = "esc / back to results";
    }
    const float titleX = (960.f - title.size() * 21.7f) * 0.5f;
    ui_.text(target, title, titleX, 119.f, 36, sf::Color(220, 190, 255));
    const float lineX = (960.f - line.size() * 10.85f) * 0.5f;
    ui_.text(target, line, lineX, 185.f, 18);
    const float detailX = (960.f - detail.size() * 9.64f) * 0.5f;
    ui_.text(target, detail, detailX, 225.f, 16, sf::Color(185, 175, 205));
    if (state_ == GameState::Credits) {
        ui_.text(target, "abdullah / screens sound and integration", 292.f, 261.f, 16);
        ui_.text(target, "font: dejavu sans mono / license in assets/fonts", 259.f, 294.f, 14);
    } else if (state_ == GameState::Dead && patches_.getActiveCount() > 0) {
        ui_.text(target, patchMessage_, 80.f, 273.f, 14, sf::Color(240, 190, 255));
    }
    ui_.button(target, first, 290.f, 330.f, 380.f, 48.f);
    ui_.button(target, second, 290.f, 390.f, 380.f, 48.f);
    ui_.text(target, sounds_.isMuted() ? "s / turn sound on" : "s / mute sound", 400.f, 478.f, 14,
             sf::Color(175, 165, 195));
}

void Game::drawMenu(sf::RenderTarget& target) const {
    ui_.monitor(target, 456.f, 42.f, false);
    if (menuPage_ == MenuPage::Main) {
        ui_.text(target, "ragebait maxxing", 309.f, 105.f, 36, sf::Color(220, 190, 255));
        ui_.text(target, "a game with your best interests at heart", 264.f, 163.f, 17);
        int chapter = progress_.getHighestUnlocked();
        if (chapter > 3) chapter = 3;
        ui_.button(target, "enter / continue chapter " + to_string(chapter), 290.f, 235.f, 380.f, 46.f);
        ui_.button(target, "l / level select", 290.f, 293.f, 380.f, 46.f);
        ui_.button(target, "o / settings", 290.f, 351.f, 380.f, 46.f);
        ui_.button(target, "q / quit while you can", 290.f, 409.f, 380.f, 46.f);
        return;
    }
    if (menuPage_ == MenuPage::Settings) {
        ui_.text(target, "reliable settings", 320.f, 125.f, 32, sf::Color(220, 190, 255));
        ui_.text(target, "these buttons are legally required to work", 274.f, 195.f, 16);
        ui_.button(target, sounds_.isMuted() ? "s / sound off" : "s / sound on", 290.f, 285.f, 380.f, 48.f);
        ui_.button(target, "esc / back", 290.f, 355.f, 380.f, 48.f);
        return;
    }
    ui_.text(target, "pick your suffering", 307.f, 105.f, 34, sf::Color(220, 190, 255));
    for (int i = 0; i < 6; ++i) {
        const int chapter = i + 1;
        const float x = i % 2 == 0 ? 190.f : 490.f;
        const float y = 220.f + (i / 2) * 62.f;
        string words = to_string(chapter) + " / ";
        if (chapter > 3) words += "coming soon";
        else if (!progress_.isUnlocked(chapter)) words += "locked";
        else if (progress_.isCompleted(chapter)) words += "cleared";
        else words += "available";
        ui_.button(target, words, x, y, 280.f, 46.f);
    }
    ui_.button(target, "esc / back", 290.f, 420.f, 380.f, 44.f);
}

void Game::draw(sf::RenderTarget& target) const {
    level_.draw(target);
    if (state_ != GameState::Menu) player_.draw(target);
    if (hasGhost()) {
        const sf::FloatRect ghost = getGhostBounds();
        ui_.box(target, ghost.left, ghost.top, ghost.width, ghost.height, sf::Color(130, 210, 255, 95));
    }
    if (interfacePopupEnabled_ && state_ != GameState::Menu) popup_.draw(target, ui_);
    if (state_ != GameState::Menu) drawHud(target);
    if (state_ != GameState::Playing) drawScreen(target);
}

GameState Game::getState() const { return state_; }
