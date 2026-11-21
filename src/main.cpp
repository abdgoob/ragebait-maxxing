#include "game/Game.h"
#include <SFML/Graphics.hpp>

using namespace std;

int main(int argc, char* argv[]) {
    sf::RenderWindow window(sf::VideoMode(960, 540), "ragebait maxxing");
    window.setFramerateLimit(60);
    // holding R should not keep respawning the player
    window.setKeyRepeatEnabled(false);
    // assets are copied beside the executable, also works when launched elsewhere
    string executable = argc > 0 ? argv[0] : "";
    const string::size_type slash = executable.find_last_of("/\\");
    const string executableFolder = slash == string::npos ? "" : executable.substr(0, slash + 1);
    const string assetFolder = executableFolder + "assets/";
    const string savePath = executableFolder + "ragebait_save.txt";
    Game game(assetFolder, savePath);
    if (!game.isReady()) return 1;
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        bool changedState = false;
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::LostFocus) {
                game.lostFocus();
                changedState = true;
            }
            if (event.type == sf::Event::Resized) {
                // keep the same game coords and button hitboxes when resized
                if (event.size.width > 0 && event.size.height > 0) {
                    sf::View view(sf::FloatRect(0.f, 0.f, 960.f, 540.f));
                    const float ratio = static_cast<float>(event.size.width) / event.size.height;
                    if (ratio > 960.f / 540.f) {
                        const float width = (960.f / 540.f) / ratio;
                        view.setViewport(sf::FloatRect((1.f - width) * 0.5f, 0.f, width, 1.f));
                    } else {
                        const float height = ratio / (960.f / 540.f);
                        view.setViewport(sf::FloatRect(0.f, (1.f - height) * 0.5f, 1.f, height));
                    }
                    window.setView(view);
                }
            }
            if (event.type == sf::Event::MouseMoved) {
                game.mouseMoved(window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)));
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                game.mouseReleased();
            }
            if (event.type == sf::Event::MouseButtonPressed && window.hasFocus()
                && event.mouseButton.button == sf::Mouse::Left) {
                game.mousePressed(window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)));
                changedState = true;
            }
            if (event.type != sf::Event::KeyPressed || !window.hasFocus()) continue;
            if (game.getState() == GameState::Playing
                && (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right
                    || event.key.code == sf::Keyboard::Space)) continue;
            game.handleKey(event.key.code);
            changedState = true;
        }
        if (game.wantsQuit()) window.close();
        if (!window.isOpen()) break;
        if (!changedState && window.hasFocus()) {
            bool left = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
            bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
            bool jump = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
            game.update(dt, left, right, jump);
        }
        window.clear(sf::Color(25, 22, 35));
        game.draw(window);
        window.display();
    }
    return 0;
}
