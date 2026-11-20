cmake_minimum_required(VERSION 3.16)
project(ragebait_maxxing CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

set(GAME_SOURCES
    src/game/Game.cpp
    src/game/AttemptRecorder.cpp
    src/ui/GameUI.cpp
    src/ui/PopupWindow.cpp
    src/audio/SoundSystem.cpp
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

add_executable(ragebait_maxxing src/main.cpp ${GAME_SOURCES})
target_include_directories(ragebait_maxxing PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ragebait_maxxing PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)

add_custom_command(TARGET ragebait_maxxing POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:ragebait_maxxing>/assets"
)

option(RAGEBAIT_BUILD_TESTS "build the game flow checks" OFF)
if(RAGEBAIT_BUILD_TESTS)
    enable_testing()
    add_executable(game_flow_checks tests/game_flow.cpp ${GAME_SOURCES})
    target_include_directories(game_flow_checks PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(game_flow_checks PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)
    add_custom_command(TARGET game_flow_checks POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:game_flow_checks>/assets"
    )
    add_test(NAME game_flow_checks COMMAND game_flow_checks)
    set_tests_properties(game_flow_checks PROPERTIES WORKING_DIRECTORY "$<TARGET_FILE_DIR:game_flow_checks>")
endif()
