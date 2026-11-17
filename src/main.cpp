#include "game/Game.h"
#include <SFML/Graphics.hpp>

using namespace std;

int main() {
    sf::RenderWindow window(sf::VideoMode(960, 540), "ragebait maxxing");
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
    Game game;
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        bool changedState = false;
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::LostFocus) {
                game.pause();
                changedState = true;
            }
            if (event.type != sf::Event::KeyPressed || !window.hasFocus()) continue;
            if (event.key.code == sf::Keyboard::Enter) game.start();
            else if (event.key.code == sf::Keyboard::Escape) {
                if (game.getState() == GameState::Paused) game.resume();
                else game.pause();
            } else if (event.key.code == sf::Keyboard::R) game.retry();
            else if (event.key.code == sf::Keyboard::M) game.returnToMenu();
            else continue;
            changedState = true;
        }
        if (!window.isOpen()) break;
        if (!changedState && window.hasFocus()) {
            game.update(dt, sf::Keyboard::isKeyPressed(sf::Keyboard::Left),
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Right),
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Space));
        }
        window.clear(sf::Color(25, 22, 35));
        game.draw(window);
        window.display();
    }
    return 0;
}
