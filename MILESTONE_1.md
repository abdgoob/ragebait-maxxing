# Milestone 1: game flow logic

Game owns the sandbox player, level, director and patches. Level owns its traps;
PatchManager owns its patches. Game, Level and PatchManager cannot be copied.
There are no menu screens yet: state changes and instructions print to the console.

## Controls

- Enter: start a fresh session from the menu.
- Left/right and Space: move and jump while playing.
- Escape: pause or resume.
- R: retry while playing or dead, preserving deaths and active patches.
- M: return to the logical menu. Enter then resets the session.
- Losing window focus pauses play. Escape resumes after focus returns.

Discrete actions use key events with keyboard repeat disabled. Updates are skipped
on transition frames, so time spent paused is not applied when resuming.

## Integration

Game exposes start, pause, resume, retry, returnToMenu, update, draw and getState.
Future UI can call these functions without owning the gameplay objects.
Level::update now advances the player after gravity and before trap checks;
callers must not also call Player::update for the same frame.
Retry resets traps and respawns through DeveloperDirector without clearing patches.
Starting a new session clears patches before resetting the player and director.

## Manual verification with SFML 2.x

Build using the existing CMake instructions, then run from a console.

1. Before Enter, movement keys do nothing. Enter starts at the original spawn.
2. Arrows move the player and Space jumps. Touching a spike enters Dead once.
3. Hold R: only one retry occurs. Release and press again for another retry.
4. Pause while moving: player, traps and patch timers freeze. R does nothing
   while paused. Resume and check there is no time jump.
5. Die twice to trigger a control patch. Retry and check controls remain flipped;
   active play eventually expires the timed patch. Dead/paused time must not count.
6. Return to menu from playing, paused and dead. Enter starts fresh with zero
   deaths, normal controls/gravity, reset traps and fresh director reactions.
7. Switch to another window: play pauses. Return and press Escape to resume.
8. Close the window from any state.

## Verification on the implementation machine

Git whitespace checks passed. A MinGW compiler is available, but compilation
stops at the missing SFML/Graphics.hpp dependency; CMake is also unavailable.
The build and interactive checks above remain unverified on this machine.
Milestones 2 through 5 are deferred to later requests.
