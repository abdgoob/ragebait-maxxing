#include "game/Game.h"
#include "patches/ControlPatch.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iostream>

using namespace std;

const char* kTestSave = "game_flow_test_save.txt";
const char* kBadSave = "game_flow_bad_save.txt";

void check(bool ok, const char* message) {
    if (ok) return;
    cerr << "failed: " << message << "\n";
    exit(1);
}

void snapshot(Game& game, sf::RenderTexture& target, const string& name) {
    target.clear(sf::Color(25, 22, 35));
    game.draw(target);
    target.display();
    check(target.getTexture().copyToImage().saveToFile(name + ".png"), "screen capture");
}

void checkRecording() {
    AttemptRecorder attempts;
    attempts.startAttempt();
    attempts.record(Position(1.f, 2.f));
    attempts.clear();
    attempts.startAttempt(Position(0.f, 0.f));
    attempts.record(0.02f, Position(20.f, 40.f));
    attempts.record(0.04f, Position(60.f, 80.f));
    attempts.finishAttempt();
    attempts.startAttempt(Position(0.f, 0.f));
    check(attempts.hasGhost(), "last attempt survives retry");
    attempts.updateReplay(0.04f);
    check(attempts.getGhostPosition() == Position(40.f, 60.f), "replay follows elapsed time across different frame sizes");
    attempts.updateReplay(1.f);
    check(attempts.getGhostPosition() == Position(60.f, 80.f), "replay holds its final position");
    attempts.startAttempt(Position(0.f, 0.f));
    check(attempts.getGhostPosition() == Position(0.f, 0.f), "retry rewinds the ghost");
    for (int i = 0; i < AttemptRecorder::MAX_FRAMES + 10; ++i) attempts.record(0.01f, Position(1.f, 1.f));
    check(attempts.isRecordingFull(), "long attempts stop at the array limit");
    attempts.clear();
    check(!attempts.hasGhost() && !attempts.isRecordingFull(), "new chapter clears recording");
}

void checkGhostReplay() {
    GhostReplay ghost(Position(10.f, 20.f));
    check(!ghost.isVisible(), "ghost starts hidden");
    ghost.setVisible(true);
    check(ghost.isVisible() && ghost.getBounds().left == 10.f, "ghost exposes its future switch bounds");
    ghost.setPosition(Position(30.f, 40.f));
    check(ghost.getPosition() == Position(30.f, 40.f), "ghost presentation follows recorder position");
    ghost.reset();
    check(!ghost.isVisible(), "ghost reset hides it");
}

void checkPopup() {
    PopupWindow popup;
    check(!popup.isEnabled() && !popup.isOpen(), "popup starts disabled outside its chapter");
    popup.setEnabled(true);
    check(popup.isOpen() && !popup.isPlatform(), "enabled popup starts open");
    check(popup.press(sf::Vector2f(600.f, 240.f)), "title bar accepts drag");
    popup.move(sf::Vector2f(310.f, 320.f));
    check(!popup.isPlatform(), "dragging is not a platform yet");
    popup.release();
    check(popup.isPlatform(), "dropped popup exposes a platform");
    check(fabs(popup.getPlatformBounds().left - 250.f) < 0.01f, "drag keeps the original grab offset");
    popup.reset();
    popup.setEnabled(true);
    popup.press(sf::Vector2f(830.f, 238.f));
    check(popup.isOpen(), "close button dodges first click");
    popup.press(sf::Vector2f(565.f, 238.f));
    popup.press(sf::Vector2f(830.f, 238.f));
    popup.press(sf::Vector2f(565.f, 238.f));
    check(!popup.isOpen() && !popup.isPlatform(), "close eventually works");
    popup.setEnabled(false);
    check(!popup.isEnabled() && !popup.press(sf::Vector2f(550.f, 230.f)), "disabled popup ignores input");
}

void checkPatchMessages() {
    Player player;
    Level level;
    PatchManager patches;
    patches.queuePatch(new ControlPatch());
    patches.applyQueued(player, level);
    check(patches.getAnnouncementCount() == 1, "installed patch gets a notification");
    check(patches.getLastVersion() == "v0.0.4-UX", "notification uses the actual patch version");
    check(!patches.getLastAnnouncement().empty(), "notification includes patch notes");
    check(patches.getActiveCount() == 1 && player.areControlsFlipped(), "presentation sees the active patch");
    patches.update(1.f, player, level);
    check(fabs(patches.getRemainingTime() - 9.f) < 0.01f, "hud reads the manager countdown");
    patches.clearAll(player, level);
    check(patches.getActiveCount() == 0 && !player.areControlsFlipped(), "clearing patches restores controls");
}

void checkProgressSave() {
    remove(kTestSave);
    remove(kBadSave);
    ProgressSave fresh;
    check(fresh.getHighestUnlocked() == 1, "fresh save unlocks chapter one");
    check(!fresh.isMuted() && fresh.getTotalDeaths() == 0, "fresh save uses safe settings");
    fresh.completeChapter(1, 7);
    fresh.setMuted(true);
    fresh.addDeath();
    fresh.addRetry();
    check(fresh.save(kTestSave), "progress save writes");
    ProgressSave loaded;
    check(loaded.load(kTestSave), "progress save reads");
    check(loaded.getHighestUnlocked() == 2 && loaded.isCompleted(1), "chapter unlock survives reload");
    check(loaded.getBestDeaths(1) == 7, "best deaths survives reload");
    check(loaded.isMuted() && loaded.getTotalDeaths() == 1 && loaded.getTotalRetries() == 1,
          "settings and totals survive reload");
    ofstream bad(kBadSave);
    bad << "RAGEBAIT_SAVE 99\nthis is from the future\n";
    bad.close();
    check(!loaded.load(kBadSave), "newer or malformed save is rejected");
    check(loaded.getHighestUnlocked() == 1 && !loaded.isMuted(), "bad save falls back safely");
    remove(kTestSave);
    remove(kBadSave);
}

void checkFrozenScreen(Game& game, sf::RenderTexture& target) {
    target.clear();
    game.draw(target);
    target.display();
    const sf::Image before = target.getTexture().copyToImage();
    for (int i = 0; i < 300; ++i) game.update(0.05f, false, true, true);
    target.clear();
    game.draw(target);
    target.display();
    const sf::Image after = target.getTexture().copyToImage();
    check(memcmp(before.getPixelsPtr(), after.getPixelsPtr(), 960 * 540 * 4) == 0,
          "paused and dead screens freeze world, counters and patch countdown");
}

int main() {
    checkProgressSave();
    checkRecording();
    checkGhostReplay();
    checkPopup();
    checkPatchMessages();
    sf::RenderTexture target;
    check(target.create(960, 540), "real sfml render target");
    remove(kTestSave);
    Game game("assets/", kTestSave);
    check(game.isReady(), "bundled font loads");
    check(game.getState() == GameState::Menu, "starts at menu");
    snapshot(game, target, "menu");
    game.mousePressed(sf::Vector2f(480.f, 255.f));
    check(game.getState() == GameState::Playing, "start button works");
    check(game.getCurrentChapter() == 1, "fresh game starts chapter one");
    snapshot(game, target, "playing");
    game.handleKey(sf::Keyboard::Escape);
    game.update(0.05f, false, true, true);
    check(game.getState() == GameState::Paused, "pause freezes gameplay");
    snapshot(game, target, "paused");
    game.handleKey(sf::Keyboard::R);
    check(game.getState() == GameState::Paused, "retry does not bypass pause");
    game.mousePressed(sf::Vector2f(480.f, 350.f));
    check(game.getState() == GameState::Playing, "resume button works");
    for (int i = 0; i < 600 && game.getState() == GameState::Playing; ++i) {
        game.update(1.f / 60.f, false, true, false);
    }
    check(game.getState() == GameState::Dead, "walking into a trap opens death screen");
    snapshot(game, target, "dead");
    game.handleKey(sf::Keyboard::Enter);
    check(game.getState() == GameState::Playing, "enter retries after death");
    game.enableGhostReplay();
    for (int i = 0; i < 600 && game.getState() == GameState::Playing; ++i) {
        game.update(1.f / 60.f, false, true, false);
    }
    check(game.getState() == GameState::Dead, "second death queues the existing patch");
    snapshot(game, target, "dead_patch");
    checkFrozenScreen(game, target);
    game.retry();
    check(game.hasGhost(), "ghost integration can use the previous attempt");
    snapshot(game, target, "ghost_hook");
    snapshot(game, target, "playing_patch");
    game.lostFocus();
    check(game.getState() == GameState::Paused, "losing focus pauses");
    checkFrozenScreen(game, target);
    game.resume();
    game.enableInterfacePopup();
    game.mousePressed(sf::Vector2f(600.f, 240.f));
    game.mouseMoved(sf::Vector2f(310.f, 320.f));
    game.mouseReleased();
    check(game.hasPopupPlatform(), "game exposes the dropped popup to level collision code");
    snapshot(game, target, "popup_hook");
    game.showFakeVictory();
    check(game.getState() == GameState::FakeVictory, "fake victory has its own frozen screen");
    snapshot(game, target, "fake_victory");
    game.handleKey(sf::Keyboard::Enter);
    check(game.getState() == GameState::Playing, "accepting fake victory returns to the same attempt");
    check(game.hasPopupPlatform(), "fake victory keeps the current interaction state");
    for (int chapter = 1; chapter <= 3; ++chapter) {
        game.completeChapter();
        check(game.getState() == GameState::ChapterClear, "exit hook opens chapter review");
        game.update(0.05f, false, false, false);
        snapshot(game, target, "review" + to_string(chapter));
        game.continueChapter();
        check(game.getState() == (chapter < 3 ? GameState::Playing : GameState::Victory), "only existing chapters advance");
        check(game.getHighestUnlockedChapter() >= chapter + (chapter < 3 ? 1 : 0), "chapter completion unlocks progress");
    }
    snapshot(game, target, "build_cleared");
    game.handleKey(sf::Keyboard::Enter);
    check(game.getState() == GameState::Credits, "credits follow completion");
    snapshot(game, target, "credits");
    game.handleKey(sf::Keyboard::Escape);
    check(game.getState() == GameState::Victory, "credits returns to results");
    game.returnToMenu();
    game.start();
    snapshot(game, target, "fresh_run");
    check(!game.hasGhost() && !game.hasPopupPlatform(), "new game clears future chapter interactions");
    game.win();
    check(game.getState() == GameState::Victory, "boss hook opens final victory");
    snapshot(game, target, "final_victory");
    game.returnToMenu();
    game.mousePressed(sf::Vector2f(480.f, 430.f));
    check(game.wantsQuit(), "quit button works");
    SoundSystem sounds;
    check(!sounds.isMuted(), "sound starts enabled");
    sounds.toggleMute();
    check(sounds.isMuted(), "sound can be muted");
    sounds.play(SoundCue::Death);
    remove(kTestSave);
    cout << "game flow checks passed\n";
    return 0;
}
