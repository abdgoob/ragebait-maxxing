#pragma once

#include "stubs/Level.h"

using namespace std;

// arsalan layouts for chapters 1-3, hussain can swap in real exits later
// numbers are just screen coords, ground top is 480

class LevelBuilder {
public:
    static void fillChapter(Level& level, int chapter);
    static const char* chapterName(int chapter);

private:
    static void chapterOne(Level& level);
    static void chapterTwo(Level& level);
    static void chapterThree(Level& level);
};