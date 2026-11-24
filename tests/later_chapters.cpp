#include "game/Game.h"
#include "levels/LevelBuilder.h"
#include <cstdio>

void check(bool ok, const char* message);
void snapshot(Game& game, sf::RenderTexture& target, const string& name);
void checkFrozenScreen(Game& game, sf::RenderTexture& target);

void checkLaterChapters(sf::RenderTexture& target) {
    const char* save = "later_chapter_test_save.txt";
    ProgressSave progress;
    progress.unlockChapter(6);
    progress.setMuted(true);
    check(progress.save(save), "later chapter test save");
    Game game("assets/", save);
    check(!game.startChapter(7), "out of range chapter never loads a fallback layout");
    check(game.startChapter(4), "unlocked ghost chapter starts");
    snapshot(game, target, "chapter4");
    for (int i = 0; i < 40; ++i) game.update(1.f / 60, false, true, false);
    for (int i = 0; i < 20; ++i) game.update(1.f / 60, false, false, false);
    check(game.getPlayerBounds().left >= 200 && game.getPlayerBounds().left < 290, "recorded attempt ends on plate");
    game.retry();
    check(game.hasGhost(), "chapter four automatically records and replays attempts");
    game.pause();
    checkFrozenScreen(game, target);
    game.resume();
    bool jump = false;
    for (int i = 0; i < 600 && game.getState() == GameState::Playing; ++i) {
        if (jump && game.isPlayerOnGround()) jump = false;
        if (i == 49 || i == 87 || i == 173 || i == 238) jump = true;
        game.update(1.f / 60, false, true, jump);
    }
    check(game.getState() == GameState::ChapterClear, "real ghost holds plate while player passes door and exits");
    game.continueChapter();
    check(game.getCurrentChapter() == 5 && !game.hasGhost(), "chapter five clears ghost and starts popup");
    snapshot(game, target, "chapter5");
    game.mousePressed(sf::Vector2f(600, 240));
    game.mouseMoved(sf::Vector2f(410, 440)); // drop left edge at 350, top at 420
    game.mouseReleased();
    check(game.hasPopupPlatform(), "chapter five popup automatically enabled and droppable");
    game.retry();
    check(!game.hasPopupPlatform(), "retry removes old popup placement");
    game.mousePressed(sf::Vector2f(600, 240));
    game.mouseMoved(sf::Vector2f(410, 440));
    game.mouseReleased();
    check(game.hasPopupPlatform(), "retry restores a usable popup");
    snapshot(game, target, "chapter5_bridge");
    int jumpCount = 0;
    jump = false;
    for (int i = 0; i < 600 && game.getState() == GameState::Playing; ++i) {
        const sf::FloatRect box = game.getPlayerBounds();
        if (jumpCount == 0 && box.left > 270) { jump = true; ++jumpCount; }
        if (jumpCount == 1 && box.left > 470) jump = false;
        if (jumpCount == 1 && box.left > 650) { jump = true; ++jumpCount; }
        if (jumpCount == 2 && box.left > 810) jump = false;
        game.update(1.f / 60, false, true, jump);
    }
    check(game.getState() == GameState::ChapterClear, "popup bridge and two jumps reach the real chapter five exit");
    game.continueChapter();
    check(game.getCurrentChapter() == 6 && !game.hasPopupPlatform(), "chapter six clears popup state");
    snapshot(game, target, "chapter6");
    game.pause();
    checkFrozenScreen(game, target);
    game.resume();
    bool right = true;
    bool fakeSeen = false;
    int roundOneFlips = 0;
    int roundTwoFlips = 0;
    bool checkedReversePause = false;
    for (int i = 0; i < 2400 && game.getState() != GameState::Victory; ++i) {
        if (game.getState() == GameState::FakeVictory) {
            fakeSeen = true;
            snapshot(game, target, "boss_fake_victory");
            const float before = game.getRunSummary().getPlayTime();
            for (int j = 0; j < 100; ++j) game.update(0.05f, false, true, false);
            check(game.getRunSummary().getPlayTime() == before, "fake boss win freezes active time");
            game.handleKey(sf::Keyboard::Enter);
            check(game.getState() == GameState::Playing, "boss fake win returns to phase two");
        }
        check(game.getState() != GameState::Dead, "boss warning lanes leave a real dodge route");
        // read the visible warning and dodge toward the room's centre
        right = game.getBossWarningX() < 450.f;
        if (i > 1200) right = true; // shutdown phase, run to the green switch
        const bool keyRight = right != game.areBossControlsReversed();
        const bool reversedBefore = game.areBossControlsReversed();
        game.update(1.f / 60, !keyRight, keyRight, false);
        if (reversedBefore != game.areBossControlsReversed()) {
            if (fakeSeen) ++roundTwoFlips;
            else ++roundOneFlips;
        }
        if (game.areBossControlsReversed() && !checkedReversePause) {
            game.pause();
            checkFrozenScreen(game, target);
            check(game.areBossControlsReversed(), "pausing freezes the boss reversal state");
            game.resume();
            checkedReversePause = true;
        }
    }
    check(fakeSeen && game.getState() == GameState::Victory, "actual boss gameplay reaches fake then real victory");
    check(roundOneFlips >= 2 && roundTwoFlips >= 6, "boss repeatedly reverses controls and escalates in round two");
    check(!game.areBossControlsReversed(), "shutdown restores normal boss controls");
    check(checkedReversePause, "reversed boss controls were checked during pause");
    snapshot(game, target, "boss_real_victory");
    ProgressSave reloaded;
    check(reloaded.load(save) && reloaded.isCompleted(6), "actual boss defeat saves chapter six completion");
    check(game.getRunSummary().getChaptersCleared() == 3, "later chapter run counts three actual clears");
    game.handleKey(sf::Keyboard::Enter);
    check(game.getState() == GameState::Credits, "real boss victory leads to credits");
    remove(save);
}

void checkLaterWorld() {
    Level level;
    Player player(Position(600, 444));
    DeveloperDirector dev;
    PatchManager patches;
    LevelBuilder::fillChapter(level, 4);
    for (int i = 0; i < 60; ++i) {
        player.handleInput(false, true, false);
        level.update(1.f / 60, player, dev, patches);
    }
    check(player.getBounds().left + player.getBounds().width <= 650.f, "closed ghost door blocks horizontal movement");
    level.setInteractionBounds(true, sf::FloatRect(230, 444, 28, 36), false, sf::FloatRect());
    level.update(1.f / 60, player, dev, patches);
    check(level.isGhostDoorOpen(), "ghost bounds open the linked door");
    level.setInteractionBounds(false, sf::FloatRect(), false, sf::FloatRect());
    level.update(1.f / 60, player, dev, patches);
    check(!level.isGhostDoorOpen(), "door closes when helper leaves");
    level.resetTraps();
    check(!level.isGhostDoorOpen(), "retry closes ghost door");
    player.respawn(Position(370, 376));
    level.update(1.f / 60, player, dev, patches);
    check(player.isOnGround() && player.getPosition().getY() == 376.f, "player lands on moving platform");
    const float carriedFrom = player.getPosition().getX();
    for (int i = 0; i < 20; ++i) level.update(1.f / 60, player, dev, patches);
    check(player.isOnGround() && player.getPosition().getX() > carriedFrom + 5.f, "moving platform carries its rider");

    LevelBuilder::fillChapter(level, 5);
    player.respawn(Position(400, 444));
    for (int i = 0; i < 100 && player.isAlive(); ++i) level.update(0.01f, player, dev, patches);
    check(!player.isAlive(), "chapter five gap kills without a popup bridge");
    patches.clearAll(player, level);
    LevelBuilder::fillChapter(level, 6);
    player.respawn(Position(200, 444));
    for (int i = 0; i < 90 && player.isAlive(); ++i) level.update(1.f / 60, player, dev, patches);
    check(!player.isAlive(), "boss attack hurts a player who ignores the warning");
    level.resetTraps();
    check(level.getBoss().getPhase() == 0 && !level.getBoss().isDefeated(), "retry resets the entire boss fight");
    patches.clearAll(player, level);
    DeveloperBoss boss;
    player.respawn(Position(100, 444));
    bool warningSeen = false;
    for (int i = 0; i < 170; ++i) {
        // stand outside the previous warning lane so this checks timing, not dodging skill
        player.setPosition(boss.getWarningX() < 450 ? 700.f : 100.f, 444);
        boss.update(1.f / 60, player);
        if (boss.isControlWarning()) warningSeen = true;
    }
    check(player.isAlive() && warningSeen && boss.areControlsReversed(), "boss warns before its first timed reversal");
    boss.reset();
    check(!boss.areControlsReversed() && !boss.isControlWarning(), "boss reset clears reversal and warning timers");
}
