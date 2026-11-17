#include "developer/DeveloperDirector.h"
#include "patches/PatchManager.h"
#include "stubs/Level.h"
#include "stubs/Player.h"
#include "traps/TrapFactory.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

// tiny sandbox so we can show traps/patches without waiting on full game loop
// abdullah/hussan can rip this out when menus and real player land

int main() {
    sf::RenderWindow window(sf::VideoMode(960, 540), "ragebait maxxing - arsalan sandbox");
    window.setFramerateLimit(60);

    Player player(Position(80.f, 380.f));
    player.setSpawnPoint(player.getPosition());

    Level level("arsalan test pit");
    level.addTrap(TrapFactory::create(TrapKind::Spike, Position(340.f, 420.f)));
    level.addTrap(TrapFactory::create(TrapKind::FakePlatform, Position(220.f, 400.f)));
    level.addTrap(TrapFactory::create(TrapKind::ControlSwap, Position(520.f, 400.f)));

    DeveloperDirector dev;
    PatchManager patches;
    dev.onLevelStart(level);

    sf::Font font;
    if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) {
        std::cerr << "font failed, text might not show on your machine\n";
    }

    sf::Clock clock;
    std::string banner = "arrows move, space jump, R retry";

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
        }

        const float dt = clock.restart().asSeconds();

        bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
        bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
        bool jump = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            player.setControlsFlipped(false);
            player.setGravityScale(1.f);
            patches.clearAll(player, level);
            level.resetTraps();
            dev.onRetry(player);
        }

        if (player.isAlive()) {
            player.handleInput(left, right, jump);
            level.update(dt, player, dev, patches);
        } else {
            banner = dev.getLastTaunt() + " (press R)";
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                patches.applyQueued(player, level);
                dev.onRetry(player);
                level.resetTraps();
            }
        }

        if (patches.peekNextAnnouncement().size() && player.isAlive()) {
            banner = patches.peekNextAnnouncement();
        }

        window.clear(sf::Color(25, 22, 35));
        level.draw(window);
        player.draw(window);

        sf::Text hud;
        hud.setFont(font);
        hud.setCharacterSize(16);
        hud.setFillColor(sf::Color(230, 210, 255));
        hud.setPosition(12.f, 8.f);
        hud.setString(banner + " | deaths: " + std::to_string(player.getDeathCount()));
        window.draw(hud);

        window.display();
    }

    return 0;
}