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
    return "test pit";
}

void LevelBuilder::fillChapter(Level& level, int chapter) {
    level.clearTraps();
    level.setPit(0.f, 0.f);
    level.setName(chapterName(chapter));

    if (chapter == 2) chapterTwo(level);
    else if (chapter == 3) chapterThree(level);
    else chapterOne(level);
}

void LevelBuilder::chapterOne(Level& level) {
    // tutorial vibe: empty walk then a floor that is lying
    level.addTrap(TrapFactory::makeFakeFloor(Position(300.f, 390.f), 130.f, 18.f));
    level.addTrap(TrapFactory::makeSpike(Position(320.f, kSpikeY), 0.f, 0.f));
    level.addTrap(TrapFactory::makeSpike(Position(700.f, kSpikeY), 40.f, 2.2f));
}

void LevelBuilder::chapterTwo(Level& level) {
    // pit under a fake floor, moving spikes, then a purple pad
    level.setPit(430.f, 560.f);
    level.addTrap(TrapFactory::makeSpike(Position(220.f, kSpikeY), 55.f, 2.8f));
    level.addTrap(TrapFactory::makeFakeFloor(Position(425.f, kFakeY), 140.f, 24.f));
    level.addTrap(TrapFactory::makeCtrlPad(Position(620.f, kPadY)));
    level.addTrap(TrapFactory::makeSpike(Position(760.f, kSpikeY), 30.f, 3.5f));
}

void LevelBuilder::chapterThree(Level& level) {
    // tighter, patches will fire more from the director
    level.setPit(500.f, 610.f);
    level.addTrap(TrapFactory::makeSpike(Position(180.f, kSpikeY), 60.f, 3.4f));
    level.addTrap(TrapFactory::makeCtrlPad(Position(340.f, kPadY)));
    level.addTrap(TrapFactory::makeFakeFloor(Position(495.f, kFakeY), 120.f, 24.f));
    level.addTrap(TrapFactory::makeSpike(Position(720.f, kSpikeY), 45.f, 4.f));
}