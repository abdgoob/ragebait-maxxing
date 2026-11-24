#pragma once

#include "stubs/Level.h"

using namespace std;

// six basic layouts, world objects and exits are connected by level
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
