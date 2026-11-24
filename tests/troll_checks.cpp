#include "levels/LevelBuilder.h"
#include "stubs/Player.h"
#include "developer/DeveloperDirector.h"
#include "patches/PatchManager.h"
#include "patches/ControlPatch.h"
#include "patches/GravityPatch.h"

void check(bool ok, const char* message);

void checkPlayableRoutes() {
    // recorded key presses through the actual layouts, no teleporting in this check
    const int jumpFrames[3][5] = { {35, 96, 150, -1, -1}, {24, 87, 153, 208, -1}, {56, 132, 212, 276, 335} };
    const int waits[3] = {0, 0, 115}; // chapter 3 needs a different moving-spike phase
    for (int chapter = 1; chapter <= 3; ++chapter) {
        Level level;
        Player player(Position(80, 444));
        DeveloperDirector dev;
        PatchManager patches;
        LevelBuilder::fillChapter(level, chapter);
        bool jump = false;
        bool won = false;
        for (int frame = 0; frame < 500 && player.isAlive(); ++frame) {
            if (player.isOnGround() && jump) jump = false;
            for (int i = 0; i < 5; ++i) if (frame == jumpFrames[chapter - 1][i]) jump = true;
            const bool moving = frame >= waits[chapter - 1];
            player.handleInput(moving && player.areControlsFlipped(), moving && !player.areControlsFlipped(), jump);
            level.update(1.f / 60.f, player, dev, patches);
            if (level.takeEvent() == LevelEvent::RealExit) { won = true; break; }
        }
        check(won && player.isAlive(), "each actual chapter has a playable route through its betrayal");
        patches.clearAll(player, level);
    }
}

// these checks use the same level update as the game, no fake physics here
void checkTrolls() {
    checkPlayableRoutes();
    {
        Level level;
        Player player(Position(80, 444));
        DeveloperDirector dev;
        PatchManager patches;
        LevelBuilder::fillChapter(level, 1);
        // standing on the disguised floor must uncover an actual hole, not safe ground
        player.setPosition(325, 444);
        for (int i = 0; i < 100 && player.isAlive(); ++i) level.update(0.01f, player, dev, patches);
        check(!player.isAlive(), "chapter one disguised floor drops the player into a real pit");
        level.resetTraps();
        player.respawn();
        player.setPosition(325, 444);
        level.update(0.01f, player, dev, patches);
        check(player.isAlive() && player.getPosition().getY() < 450.f, "retry restores the disguised pit lid");
        patches.clearAll(player, level);
    }
    for (int chapter = 1; chapter <= 3; ++chapter) {
        Level level;
        Player player(Position(80.f, 444.f));
        DeveloperDirector dev;
        PatchManager patches;
        LevelBuilder::fillChapter(level, chapter);
        dev.onLevelStart(level);
        const float flagX = chapter == 1 ? 560.f : (chapter == 2 ? 590.f : 640.f);
        player.setPosition(flagX, 444.f);
        level.update(0.01f, player, dev, patches);
        check(player.isAlive(), "checkpoint placement is safe");
        check(level.takeEvent() == (chapter == 2 ? LevelEvent::FakeCheckpoint : LevelEvent::Checkpoint),
              "checkpoint reports the right event");
        check(player.getSpawnPoint().getX() == (chapter == 2 ? 80.f : flagX),
              "fake checkpoint never overwrites the actual spawn");
        level.update(0.01f, player, dev, patches);
        check(level.takeEvent() == LevelEvent::None, "checkpoint does not spam events every frame");

        // touching the real end before the prank is finished cannot skip the chapter
        player.setPosition(915.f, 444.f);
        level.update(0.01f, player, dev, patches);
        check(level.takeEvent() == LevelEvent::None, "old right edge cannot skip the fake exit");
        player.setPosition(810.f, 400.f); // approach above the old floor spikes
        level.update(0.01f, player, dev, patches);
        check(level.takeEvent() == LevelEvent::FakeExit && level.hasBetrayed(), "every chapter has an exit betrayal");
        if (chapter == 3) check(player.areControlsFlipped(), "chapter three installs its betrayal patch");

        // wait on safe ground while the short warning runs out
        player.setPosition(915.f, 444.f);
        for (int i = 0; i < 50; ++i) level.update(0.01f, player, dev, patches);
        check(player.isAlive(), "betrayal leaves a safe reachable landing");
        player.setPosition(915.f, 444.f);
        level.update(0.01f, player, dev, patches);
        check(level.takeEvent() == LevelEvent::RealExit, "real exit becomes usable after betrayal");
        if (chapter == 1) {
            player.setPosition(825.f, 444.f);
            for (int i = 0; i < 100 && player.isAlive(); ++i) level.update(0.01f, player, dev, patches);
            check(!player.isAlive(), "chapter one finish floor actually collapses");
        }
        if (chapter == 2) {
            player.setPosition(860.f, 444.f);
            level.update(0.01f, player, dev, patches);
            check(!player.isAlive(), "chapter two finish spikes really hurt");
        }
        patches.clearAll(player, level);
        level.resetTraps();
        player.respawn();
        check(!level.hasBetrayed() && level.takeEvent() == LevelEvent::None, "retry clears betrayal and pending event");
        check(player.getPosition().getX() == (chapter == 2 ? 80.f : flagX), "retry respects only genuine checkpoints");
    }

    Level level;
    Player player;
    PatchManager patches;
    patches.queuePatch(new ControlPatch());
    patches.applyQueued(player, level);
    patches.queuePatch(new ControlPatch());
    patches.applyQueued(player, level);
    patches.queuePatch(new GravityPatch(1300.f));
    patches.applyQueued(player, level);
    patches.queuePatch(new GravityPatch(1500.f));
    patches.applyQueued(player, level);
    patches.update(20.f, player, level);
    check(!player.areControlsFlipped() && level.getBaseGravity() == 900.f, "overlapping punishments restore original rules");
    DeveloperDirector dev;
    dev.onLevelStart(level);
    for (int death = 1; death <= 4; ++death) {
        dev.onPlayerDeath(player, 0);
        dev.maybeDropPatch(player, level, patches);
        patches.applyQueued(player, level);
        if (death == 2) check(player.areControlsFlipped(), "second death punishes with reversed controls");
    }
    check(level.getBaseGravity() == 1300.f, "fourth death punishes with gravity");
    patches.clearAll(player, level);
}

void checkTextures(sf::RenderTexture& target) {
    AssetManager assets("assets/");
    for (int i = 0; i < 8; ++i) {
        check(assets.getTexture(static_cast<Art>(i)) != 0, "bundled texture loads");
    }
    AssetManager missing;
    check(!missing.load("missing-test-assets/"), "missing textures fail safely");
    check(!missing.draw(target, Art::Player, sf::FloatRect(0, 0, 28, 36)), "missing texture asks caller for fallback");
    Player player(Position(80, 444));
    const sf::FloatRect before = player.getBounds();
    player.setAssets(&assets);
    check(player.getBounds() == before, "art does not change physics bounds");
    player = Player(Position(80, 444));
    player.draw(target); // new-run assignment must keep a valid borrowed asset pointer
    Level level;
    level.setAssets(&missing);
    for (int chapter = 1; chapter <= 3; ++chapter) {
        LevelBuilder::fillChapter(level, chapter);
        target.clear();
        level.draw(target);
        player.setAssets(&missing);
        player.draw(target);
        target.display();
        check(target.getTexture().copyToImage().saveToFile("fallback" + to_string(chapter) + ".png"), "fallback renders without textures");
        level.setAssets(&assets);
        target.clear();
        level.draw(target);
        player.setAssets(&assets);
        player.draw(target);
        target.display();
        check(target.getTexture().copyToImage().saveToFile("chapter_art" + to_string(chapter) + ".png"), "chapter textures render");
        level.setAssets(&missing);
    }
}
