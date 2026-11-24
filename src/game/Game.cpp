#include "game/Game.h"
#include "levels/LevelBuilder.h"
#include <iostream>

using namespace std;

Game::Game() : Game("assets/", "ragebait_save.txt") {}

Game::Game(const string& assetFolder) : Game(assetFolder, "ragebait_save.txt") {}

Game::Game(const string& assetFolder, const string& savePath)
    : assets_(assetFolder), player_(Position(80.f, 444.f)), level_("01 everything is fine"),
      state_(GameState::Menu), chapter_(1), ui_(assetFolder), savePath_(savePath), quit_(false),
      finalVictory_(false), ghostReplayEnabled_(false), interfacePopupEnabled_(false),
      shownPatch_(0), messageTimer_(0.f), patchPopupTimer_(0.f), stateTimer_(0.f),
      menuPage_(MenuPage::Main) {
    progress_.load(savePath_);
    player_.setAssets(&assets_);
    level_.setAssets(&assets_);
    sounds_.setMuted(progress_.isMuted());
    loadChapter(1);
}

Game::~Game() { patches_.clearAll(player_, level_); }

void Game::loadChapter(int n) {
    if (n < 1) n = 1;
    if (n > 6) n = 6;
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
    summary_.startChapter();
    shownPatch_ = patches_.getAnnouncementCount();
    patchPopupTimer_ = 0.f;
    stateTimer_ = 0.f;
    attempts_.clear();
    ghostReplayEnabled_ = false;
    interfacePopupEnabled_ = false;
    ghost_.reset();
    popup_.setEnabled(false);
    if (chapter_ == 4) enableGhostReplay();
    if (chapter_ == 5) enableInterfacePopup();
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
    if (chapter > 6) chapter = 6;
    startChapter(chapter);
}

bool Game::startChapter(int chapter) {
    if (state_ != GameState::Menu) return false;
    if (chapter < 1 || chapter > 6 || !progress_.isUnlocked(chapter)) return false;
    patches_.clearAll(player_, level_);
    // a new run really is fresh, retries still keep the deaths
    player_ = Player(Position(80.f, 444.f));
    summary_.reset();
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
    if (ghostReplayEnabled_ && state_ == GameState::Playing) attempts_.finishAttempt();
    patches_.applyQueued(player_, level_);
    level_.resetTraps();
    dev_.onRetry(player_);
    if (ghostReplayEnabled_) {
        attempts_.startAttempt(player_.getPosition());
        ghost_.setVisible(attempts_.hasGhost());
        if (attempts_.hasGhost()) ghost_.setPosition(attempts_.getGhostPosition());
    }
    popup_.setEnabled(interfacePopupEnabled_);
    state_ = GameState::Playing;
    progress_.addRetry();
    summary_.addRetry();
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

void Game::showFakeVictory() {
    if (state_ != GameState::Playing) return;
    state_ = GameState::FakeVictory;
    popup_.release();
    sounds_.play(SoundCue::FakeWin);
}

void Game::continueChapter() {
    if (state_ != GameState::ChapterClear) return;
    const int deaths = summary_.getChapterDeaths();
    progress_.completeChapter(chapter_, deaths);
    summary_.addChapterClear();
    saveProgress();
    sounds_.play(SoundCue::Save);
    if (chapter_ < 6) {
        loadChapter(chapter_ + 1);
        state_ = GameState::Playing;
        sounds_.play(SoundCue::Chapter);
    } else {
        finalVictory_ = true;
        state_ = GameState::Victory;
        sounds_.play(SoundCue::Victory);
    }
}

void Game::win() {
    if (state_ != GameState::Playing) return;
    if (chapter_ == 6) {
        progress_.completeChapter(6, summary_.getChapterDeaths());
        summary_.addChapterClear();
        saveProgress();
    }
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
    else if (state_ == GameState::FakeVictory) state_ = GameState::Playing;
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
sf::FloatRect Game::getPlayerBounds() const { return player_.getBounds(); }
float Game::getBossWarningX() const { return level_.getBoss().getWarningX(); }
int Game::getHighestUnlockedChapter() const { return progress_.getHighestUnlocked(); }
MenuPage Game::getMenuPage() const { return menuPage_; }
bool Game::isSoundMuted() const { return sounds_.isMuted(); }
const RunSummary& Game::getRunSummary() const { return summary_; }

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
        if (!startChapter(chapter)) sounds_.play(SoundCue::Locked);
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
        if (!startChapter(i + 1)) sounds_.play(SoundCue::Locked);
        return;
    }
    if (sf::FloatRect(290.f, 420.f, 380.f, 44.f).contains(mouse)) showMenuPage(MenuPage::Main);
}

void Game::setGhostReplayEnabled(bool on) {
    if (on) enableGhostReplay();
    else disableGhostReplay();
}
void Game::setInterfacePopupEnabled(bool on) {
    if (on) enableInterfacePopup();
    else disableInterfacePopup();
}

void Game::enableGhostReplay() {
    ghostReplayEnabled_ = true;
    attempts_.clear();
    attempts_.startAttempt(player_.getPosition());
    ghost_.reset();
}

void Game::disableGhostReplay() {
    ghostReplayEnabled_ = false;
    attempts_.clear();
    ghost_.reset();
}

void Game::enableInterfacePopup() {
    interfacePopupEnabled_ = true;
    popup_.setEnabled(true);
}

void Game::disableInterfacePopup() {
    interfacePopupEnabled_ = false;
    popup_.setEnabled(false);
}
bool Game::hasGhost() const { return ghostReplayEnabled_ && ghost_.isVisible(); }
sf::FloatRect Game::getGhostBounds() const { return ghost_.getBounds(); }
bool Game::hasPopupPlatform() const { return interfacePopupEnabled_ && popup_.isPlatform(); }
sf::FloatRect Game::getPopupPlatformBounds() const { return popup_.getPlatformBounds(); }

void Game::checkPatchMessage() {
    const int count = patches_.getAnnouncementCount();
    if (count == shownPatch_) return;
    shownPatch_ = count;
    // short gameplay hint, version numbers and patch notes stay out of the player's way
    patchMessage_.clear();
    if (player_.areControlsFlipped()) patchMessage_ = "left and right are swapped";
    if (level_.getBaseGravity() != 900.f) {
        if (!patchMessage_.empty()) patchMessage_ += " / ";
        patchMessage_ += "gravity changed";
    }
    patchPopupTimer_ = 3.f;
    sounds_.play(SoundCue::Patch);
}

void Game::update(float dt, bool left, bool right, bool jump) {
    if (dt <= 0.f) return;
    if (dt > 0.05f) dt = 0.05f;
    if (state_ == GameState::ChapterClear) { stateTimer_ += dt; return; }
    if (state_ != GameState::Playing) return;

    summary_.update(dt);
    if (messageTimer_ > 0.f) messageTimer_ -= dt;
    if (patchPopupTimer_ > 0.f) patchPopupTimer_ -= dt;
    if (ghostReplayEnabled_) {
        attempts_.updateReplay(dt);
        ghost_.setVisible(attempts_.hasGhost());
        if (attempts_.hasGhost()) ghost_.setPosition(attempts_.getGhostPosition());
    }
    const float oldYVelocity = player_.getVelocity().getY();
    player_.handleInput(left, right, jump);
    level_.setInteractionBounds(hasGhost(), getGhostBounds(), hasPopupPlatform(), getPopupPlatformBounds());
    level_.update(dt, player_, dev_, patches_);
    if (ghostReplayEnabled_) attempts_.record(dt, player_.getPosition());
    checkPatchMessage();
    if (!player_.isAlive()) {
        progress_.addDeath();
        summary_.addDeath();
        saveProgress();
        if (ghostReplayEnabled_) attempts_.finishAttempt();
        state_ = GameState::Dead;
        showMessage(dev_.getLastTaunt());
        sounds_.play(SoundCue::Death);
        return;
    }
    if (oldYVelocity >= 0.f && player_.getVelocity().getY() < 0.f) sounds_.play(SoundCue::Jump);

    const LevelEvent event = level_.takeEvent();
    if (event == LevelEvent::Checkpoint) {
        showMessage("checkpoint saved. this one actually works.");
        sounds_.play(SoundCue::Save);
    } else if (event == LevelEvent::FakeCheckpoint || event == LevelEvent::FakeExit) {
        showMessage(dev_.getLastTaunt());
        sounds_.play(SoundCue::FakeWin);
        if (event == LevelEvent::FakeExit && chapter_ == 3) showFakeVictory();
    } else if (event == LevelEvent::RealExit) completeChapter();
    else if (event == LevelEvent::BossFakeWin) {
        showMessage("nice win. anyway, phase two. your warranty is void.");
        showFakeVictory();
    } else if (event == LevelEvent::BossWin) win();
}

void Game::drawHud(sf::RenderTarget& target) const {
    ui_.box(target, 0.f, 0.f, 960.f, 60.f, sf::Color(19, 16, 29, 240));
    ui_.text(target, level_.getName(), 24.f, 12.f, 20, sf::Color(210, 185, 255));
    ui_.text(target, "deaths: " + to_string(player_.getDeathCount()), 630.f, 15.f, 18);
    ui_.text(target, sounds_.isMuted() ? "s: sound off" : "s: sound on", 790.f, 18.f, 13);
    if (state_ == GameState::Playing && patchPopupTimer_ > 0.f
        && patches_.getActiveCount() > 0 && !patchMessage_.empty()) {
        ui_.text(target, patchMessage_, 24.f, 70.f, 14, sf::Color(225, 195, 230));
    }
    if (messageTimer_ > 0.f || state_ == GameState::Dead) {
        ui_.box(target, 24.f, 103.f, 912.f, 64.f, sf::Color(38, 29, 53, 235));
        ui_.monitor(target, 36.f, 114.f, dev_.getMood() >= 4);
        ui_.text(target, "dev: " + message_, 100.f, 122.f, 15);
    }
    ui_.text(target, "arrows / a d move   space / w jump   esc pause   r retry   m menu", 24.f, 520.f, 13,
             sf::Color(175, 165, 195));
}

void Game::drawScreen(sf::RenderTarget& target) const {
    // every menu screen gets the same landscape, not a dim copy of gameplay
    ui_.menuBackdrop(target);
    if (state_ == GameState::Menu) {
        drawMenu(target);
        return;
    }
    const sf::Color accent = state_ == GameState::Dead ? sf::Color(245, 155, 139) : sf::Color(103, 234, 194);
    ui_.text(target, "RB / MAXXING", 26, 22, 12, sf::Color(112, 178, 180));
    ui_.text(target, "TRUST NOTHING. RETRY EVERYTHING.", 700, 22, 11, sf::Color(112, 178, 180));
    // a solid inset panel keeps scenery out of the words
    ui_.box(target, 184, 67, 592, 247, sf::Color(7, 17, 30));
    ui_.box(target, 180, 63, 592, 247, sf::Color(27, 46, 62));
    ui_.box(target, 184, 67, 584, 239, sf::Color(13, 28, 44));
    ui_.box(target, 184, 67, 584, 3, accent);
    ui_.centeredText(target, level_.getName(), 480, 86, 12, sf::Color(140, 170, 185));
    string title;
    string line;
    string detail;
    string first;
    string second = "MAIN MENU";
    if (state_ == GameState::Paused) {
        title = "PAUSED";
        line = "fine. the traps can wait.";
        detail = "take a breath. the traps can wait.";
        first = "KEEP GOING";
    } else if (state_ == GameState::Dead) {
        title = "YOU DIED. AGAIN.";
        line = dev_.getLastTaunt();
        detail = "deaths: " + to_string(player_.getDeathCount());
        first = "TRY AGAIN";
    } else if (state_ == GameState::ChapterClear) {
        title = chapter_ == 1 && stateTimer_ < 1.5f ? "congratulations!" : "player review";
        line = chapter_ == 1 ? "you finished the tutorial. thats it." : "chapter cleared. suspicious.";
        const int deaths = summary_.getChapterDeaths();
        detail = "chapter deaths: " + to_string(deaths) + " / rating: ";
        detail += deaths == 0 ? "probably luck" : (deaths < 5 ? "concerning" : "very committed");
        first = "CONTINUE";
    } else if (state_ == GameState::FakeVictory) {
        title = "congratulations!";
        line = "you won. please stop looking at the level.";
        detail = "victory status: extremely real";
        first = "ACCEPT VICTORY";
    } else if (state_ == GameState::Victory) {
        title = finalVictory_ ? "the final hotfix" : "current build cleared";
        line = finalVictory_ ? "you reached the switch. dev has disconnected." : "all 3 chapters done. dev is making more.";
        detail = "run deaths: " + to_string(summary_.getRunDeaths()) + " / retries: "
                 + to_string(summary_.getRetries()) + " / time: "
                 + to_string(static_cast<int>(summary_.getPlayTime())) + "s";
        first = "CREDITS";
    } else if (state_ == GameState::Credits) {
        title = "blame these people";
        line = "hussain / movement and physical world";
        detail = "arsalan / traps patches and level design";
        first = "MAIN MENU";
        second = "BACK TO RESULTS";
    }
    ui_.centeredText(target, title, 483, 120, 32, sf::Color(5, 14, 25));
    ui_.centeredText(target, title, 480, 116, 32, accent);
    const unsigned int lineSize = line.size() > 50 ? 13 : 16;
    ui_.centeredText(target, line, 480, 180, lineSize, sf::Color(220, 230, 232));
    ui_.centeredText(target, detail, 480, 218, 13, sf::Color(140, 170, 185));
    if (state_ == GameState::Credits) {
        ui_.centeredText(target, "abdullah / screens sound and integration", 480, 249, 13, sf::Color(220, 230, 232));
        ui_.centeredText(target, "chapters cleared: " + to_string(summary_.getChaptersCleared())
            + " / font: dejavu sans mono", 480, 279, 11, sf::Color(140, 170, 185));
    } else if (state_ == GameState::Dead && patches_.getActiveCount() > 0) {
        ui_.centeredText(target, patchMessage_, 480, 265, 13, accent);
    }
    ui_.menuButton(target, first, "ENT", 290, 330, 380, 48, true);
    ui_.menuButton(target, second, state_ == GameState::Credits ? "ESC" : "M", 290, 390, 380, 48);
    ui_.centeredText(target, sounds_.isMuted() ? "S / SOUND OFF" : "S / SOUND ON", 480, 458, 11, sf::Color(140, 170, 185));
    ui_.centeredText(target, "a game with your best interests at heart. allegedly.", 480, 507, 12, sf::Color(167, 150, 176));
}

void Game::drawMenu(sf::RenderTarget& target) const {
    ui_.text(target, "RB / MAXXING", 26.f, 22.f, 12, sf::Color(112, 178, 180));
    ui_.text(target, "6 CHAPTERS / ZERO REFUNDS", 714.f, 22.f, 12, sf::Color(112, 178, 180));
    ui_.centeredText(target, "a game with your best interests at heart. allegedly.",
                     480.f, 507.f, 12, sf::Color(167, 150, 176));
    if (menuPage_ == MenuPage::Main) {
        ui_.centeredText(target, "TRUST NOTHING. RETRY EVERYTHING.", 480.f, 57.f, 12, sf::Color(129, 205, 191));
        ui_.centeredText(target, "RAGEBAIT", 484.f, 87.f, 58, sf::Color(6, 15, 28));
        ui_.centeredText(target, "RAGEBAIT", 480.f, 81.f, 58, sf::Color(219, 248, 220));
        ui_.centeredText(target, "M A X X I N G", 482.f, 148.f, 27, sf::Color(6, 15, 28));
        ui_.centeredText(target, "M A X X I N G", 480.f, 144.f, 27, sf::Color(245, 155, 139));
        ui_.centeredText(target, "DEATHS " + to_string(progress_.getTotalDeaths())
                 + "   /   RETRIES " + to_string(progress_.getTotalRetries()),
                 480.f, 198.f, 12, sf::Color(140, 170, 185));
        int chapter = progress_.getHighestUnlocked();
        if (chapter > 6) chapter = 6;
        ui_.menuButton(target, "PLAY / CHAPTER " + to_string(chapter), "ENT", 290.f, 235.f, 380.f, 46.f, true);
        ui_.menuButton(target, "CHAPTER SELECT", "L", 290.f, 293.f, 380.f, 46.f);
        ui_.menuButton(target, "SETTINGS", "O", 290.f, 351.f, 380.f, 46.f);
        ui_.menuButton(target, "QUIT WHILE YOU CAN", "Q", 290.f, 409.f, 380.f, 46.f);
        assets_.draw(target, Art::Player, sf::FloatRect(186.f, 432.f, 39.f, 50.f));
        assets_.draw(target, Art::Spike, sf::FloatRect(711.f, 458.f, 56.f, 24.f));
        ui_.text(target, "probably safe", 160.f, 407.f, 10, sf::Color(113, 154, 162));
        return;
    }
    if (menuPage_ == MenuPage::Settings) {
        ui_.centeredText(target, "SETTINGS", 480, 118, 40, sf::Color(219, 248, 220));
        ui_.centeredText(target, "these buttons are legally required to work", 480, 194, 15, sf::Color(140, 170, 185));
        ui_.centeredText(target, "your preferences are saved automatically", 480, 237, 12, sf::Color(140, 170, 185));
        ui_.menuButton(target, sounds_.isMuted() ? "SOUND / OFF" : "SOUND / ON", "S", 290.f, 285.f, 380.f, 48.f, true);
        ui_.menuButton(target, "BACK", "ESC", 290.f, 355.f, 380.f, 48.f);
        return;
    }
    ui_.centeredText(target, "PICK YOUR SUFFERING", 480, 105, 35, sf::Color(219, 248, 220));
    ui_.centeredText(target, "six ways to question your decisions", 480, 162, 13, sf::Color(140, 170, 185));
    for (int i = 0; i < 6; ++i) {
        const int chapter = i + 1;
        const float x = i % 2 == 0 ? 190.f : 490.f;
        const float y = 220.f + (i / 2) * 62.f;
        string words;
        if (!progress_.isUnlocked(chapter)) words += "locked";
        else if (progress_.isCompleted(chapter)) words += "cleared";
        else words += "available";
        ui_.menuButton(target, words, to_string(chapter), x, y, 280.f, 46.f,
                       progress_.isUnlocked(chapter));
    }
    ui_.menuButton(target, "BACK", "ESC", 290.f, 420.f, 380.f, 44.f);
}

void Game::draw(sf::RenderTarget& target) const {
    level_.draw(target);
    if (state_ == GameState::Playing) {
        // little signs are part of the joke, none of these labels change collision
        if (chapter_ == 4) {
            if (!hasGhost()) ui_.text(target, "stand on plate, press R. let your ghost hold it.", 180, 335, 12);
            ui_.text(target, level_.isGhostDoorOpen() ? "door open" : "needs a helper", 608, 190, 12);
        } else if (chapter_ == 5) {
            if (!hasPopupPlatform()) {
                ui_.text(target, "drag the popup across the gap. drop it low enough to jump on.", 120, 300, 12);
                if (!popup_.isOpen()) ui_.text(target, "closed it? R brings it back.", 30, 327, 12);
            }
        } else if (chapter_ == 6) {
            const DeveloperBoss& boss = level_.getBoss();
            ui_.text(target, boss.isReadyForShutdown() ? "shutdown unlocked ->" :
                (boss.getPhase() == 0 ? "dev round 1: dodge the marked lanes" : "dev round 2: faster lanes. keep moving."),
                290, 310, 13);
        } else if (!level_.hasBetrayed()) {
            ui_.text(target, chapter_ == 2 ? "save + shortcut ->" : "totally safe ->", 532.f, 402.f, 11,
                     sf::Color(185, 205, 190));
            ui_.text(target, "finish", 800.f, 395.f, 12, sf::Color(150, 240, 170));
        } else {
            ui_.text(target, "actual exit ->", 790.f, 385.f, 12, sf::Color(245, 150, 185));
        }
    }
    if (state_ != GameState::Menu) player_.draw(target);
    if (hasGhost()) ghost_.draw(target);
    if (interfacePopupEnabled_ && state_ != GameState::Menu) popup_.draw(target, ui_);
    if (state_ != GameState::Menu) drawHud(target);
    if (state_ != GameState::Playing) drawScreen(target);
}

GameState Game::getState() const { return state_; }
