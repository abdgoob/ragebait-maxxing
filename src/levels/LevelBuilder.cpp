#include "levels/LevelBuilder.h"
#include "traps/TrapFactory.h"

using namespace std;

// ground top 480, spikes sit at 456 so teeth rest on the floor
const float kSpikeY = 456.f;
const float kPadY = 464.f;
const float kFakeY = 480.f; // lid sits on the pit, same top as the real ground

const char* LevelBuilder::chapterName(int chapter) {
    if (chapter == 1) return "01 everything is fine";
    if (chapter == 2) return "02 skill issue";
    if (chapter == 3) return "03 patch hell";
    if (chapter == 4) return "04 unpaid intern";
    if (chapter == 5) return "05 accept all cookies";
    if (chapter == 6) return "06 the final hotfix";
    return "test pit";
}

void LevelBuilder::fillChapter(Level& level, int chapter) {
    level.clearTraps();
    level.setPit(0.f, 0.f);
    level.setName(chapterName(chapter));

    if (chapter == 2) chapterTwo(level);
    else if (chapter == 3) chapterThree(level);
    else if (chapter == 1) chapterOne(level);
    else if (chapter == 4) {
        level.addTrap(TrapFactory::makeSpike(Position(470.f, kSpikeY), 35.f, 2.8f));
    } else if (chapter == 5) {
        // too wide for one jump: the annoying popup is the bridge
        level.setPit(340.f, 700.f);
        level.addTrap(TrapFactory::makeSpike(Position(750.f, kSpikeY), 12.f, 2.2f));
    }
    level.setupTrolls(chapter);
}

void LevelBuilder::chapterOne(Level& level) {
    // looks like one flat path, but this section is only a lid over a real pit
    // walking onto it drops you; learning where to jump is the actual puzzle
    level.setPit(300.f, 430.f);
    level.addTrap(TrapFactory::makeFakeFloor(Position(300.f, kFakeY), 130.f, 24.f));
    // dont put the spike under an optional floating block: bait the rushed landing instead
    // there is solid ground between the pit and spike so a careful second jump works
    level.addTrap(TrapFactory::makeSpike(Position(490.f, kSpikeY), 0.f, 0.f));
    level.addTrap(TrapFactory::makeSpike(Position(700.f, kSpikeY), 50.f, 2.8f));
}

void LevelBuilder::chapterTwo(Level& level) {
    // pit under a fake floor, moving spikes, then a purple pad
    level.setPit(430.f, 560.f);
    level.addTrap(TrapFactory::makeSpike(Position(220.f, kSpikeY), 60.f, 3.3f));
    level.addTrap(TrapFactory::makeFakeFloor(Position(425.f, kFakeY), 140.f, 24.f));
    level.addTrap(TrapFactory::makeCtrlPad(Position(620.f, kPadY)));
    level.addTrap(TrapFactory::makeSpike(Position(760.f, kSpikeY), 35.f, 4.f));
}

void LevelBuilder::chapterThree(Level& level) {
    // tighter, patches will fire more from the director
    level.setPit(500.f, 610.f);
    level.addTrap(TrapFactory::makeSpike(Position(180.f, kSpikeY), 65.f, 4.f));
    level.addTrap(TrapFactory::makeCtrlPad(Position(340.f, kPadY)));
    level.addTrap(TrapFactory::makeFakeFloor(Position(495.f, kFakeY), 120.f, 24.f));
    level.addTrap(TrapFactory::makeSpike(Position(720.f, kSpikeY), 50.f, 4.6f));
}
