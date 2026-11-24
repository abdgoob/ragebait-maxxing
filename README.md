# ragebait maxxing

a small sfml rage platformer where the game changes rules and talks back.

## current build

all six chapters are playable now. clear each chapter to unlock the next; old six-slot saves still load.

the game has a menu, level select, settings, pause and death screens, chapter reviews, patch messages, sound effects, saved progress, ghost replay state, a draggable popup and final result screens.

there are image textures now too, plus real and fake checkpoints and exits in the first three chapters. the first green door is lying. touching it starts that chapter's betrayal and reveals the real exit after a short warning.

- chapter 1: friendly start, genuine checkpoint, then the finish floor disappears
- chapter 2: fake checkpoint claims progress, fake exit installs a new spike row
- chapter 3: fake celebration rolls back into the same attempt with reversed controls and glitch bars
- chapter 4 / unpaid intern: stand on the gold plate and press r. your previous attempt replays and holds its last position so the ghost can open the tall door while you cross the moving platform and spike route.
- chapter 5 / accept all cookies: drag the popup title bar over the wide gap and drop it near floor height. its top becomes a real platform. jump onto it, then jump to the far bank. closing the window removes the bridge; r brings it back.
- chapter 6 / the final hotfix: dodge marked lanes for the first round. the victory screen lies. accept it and survive the faster second round, then touch the green shutdown switch on the right for the real ending.

betrayals reset on retry. real checkpoint spawns last for the current chapter/run only; the save file still stores chapter progression, not mid-level positions. the fake flag never modifies either save data or the real spawn.

## controls

- arrow keys or a/d move
- space or w jumps
- escape pauses or goes back
- r retries
- m returns to the menu
- s toggles sound during the game or in settings
- l opens level select from the menu
- o opens settings from the menu
- number keys select an unlocked chapter from level select
- q quits

## build

sfml 2.5 or 2.6 is required.

```text
cmake -S . -B build
cmake --build build
```

to include the game flow checks:

```text
cmake -S . -B build -DRAGEBAIT_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

cmake copies the assets folder beside the executable. `ragebait_save.txt` is also kept beside the executable. deleting that file starts progression again from chapter 1.

## teammate hooks

- `game.completeChapter()` now receives the real exit event from `Level`.
- `game.enableGhostReplay()` starts recording attempts, enabled automatically in chapter 4.
- `game.hasGhost()` and `game.getGhostBounds()` feed the replay into the chapter 4 switch.
- `game.enableInterfacePopup()` starts the chapter 5 window automatically.
- `game.hasPopupPlatform()` and `game.getPopupPlatformBounds()` feed the dropped top edge into level landing checks.
- `game.showFakeVictory()` opens a fake result and returns to the same attempt.
- `game.win()` opens the real final result.

level owns a switch, linked door and up to eight platforms using normal pointers and a fixed array. the player can land on and ride moving platforms. the popup uses one-way top collision so dragging it never pushes the player through walls. ghost and popup state clear on chapter changes. retries close the door, rewind the ghost, restore the popup and reset the boss.

DeveloperBoss uses two phase timers, one telegraphed attack lane, and one shutdown trigger. warning lanes lock onto your old position, so move away instead of standing still. attacks only run while playing. the fake celebration freezes the same fight and the real win saves chapter 6 before credits.

## art and viva notes

`AssetManager` owns three textures in a fixed array. `Game` owns the manager before the other members, so its textures stay alive until those objects are gone. each object borrows a pointer and keeps its original collision bounds. a missing image just means it draws its old shape.

`assets/images/sprites.png` is a six-picture sheet: player, spikes, fake floor, control pad, checkpoint and exit. `AssetManager::getRect` holds the crop rectangles. replace the sheet with the same layout, or update those six rectangles. `background.png` is tinted per chapter and `menu.png` goes behind menu text. the bundled sprites came from image generation; the backgrounds are original simple sfml scenery from `tools/make_backgrounds.cpp`.

the comments beside each class explain the ownership and gameplay hooks for the viva. these layouts are our own, not verified exact copies of level devil 32 or 35.

the test executable also checks checkpoint spawns, betrayals, the real exit gate, retry reset, stacked patch cleanup and texture fallback. later_chapters.cpp plays chapter 4 using an actual recorded ghost, drags the chapter 5 bridge and crosses it, dodges both boss rounds and reaches real victory. it checks boss damage, locked doors, gap deaths and chapter 6 save reload too. screenshot pngs are written next to the test executable.

dejavu sans mono and its license are in `assets/fonts`.
