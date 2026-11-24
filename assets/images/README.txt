images for ragebait maxxing

sprites.png: generated original six-picture sprite sheet for this project.
background.png and menu.png: original sfml rectangle scenery, source in tools/make_backgrounds.cpp.
no level devil art was downloaded or copied.

AssetManager loads these beside the executable. cmake copies the whole assets folder there.
missing or unreadable images fall back to the old shapes. the font still needs to be present for text.
the sheet has its own dark backdrop; it is not a fully transparent sprite atlas.
the current tight crop rectangles are in AssetManager::getRect.

to regenerate the backgrounds with the local windows setup, from the repo root:
g++ -std=c++17 tools/make_backgrounds.cpp -Ibuild/sfml-install/include -Lbuild/sfml-install/lib -lsfml-graphics -lsfml-window -lsfml-system -o build/game/make_backgrounds.exe
.\build\game\make_backgrounds.exe

then build again so cmake copies the updated images next to the game.
