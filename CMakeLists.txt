cmake_minimum_required(VERSION 3.16)
project(ragebait_maxxing CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

set(SOURCES
    src/main.cpp
    src/game/Game.cpp
    src/core/Position.cpp
    src/core/GameObject.cpp
    src/stubs/Player.cpp
    src/stubs/Level.cpp
    src/traps/Trap.cpp
    src/traps/SpikeTrap.cpp
    src/traps/FakePlatform.cpp
    src/traps/ControlTrap.cpp
    src/traps/TrapFactory.cpp
    src/patches/PatchEffect.cpp
    src/patches/GravityPatch.cpp
    src/patches/ControlPatch.cpp
    src/patches/PatchManager.cpp
    src/developer/DeveloperDirector.cpp
    src/levels/LevelBuilder.cpp
)

add_executable(ragebait_maxxing ${SOURCES})
target_include_directories(ragebait_maxxing PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ragebait_maxxing PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)
