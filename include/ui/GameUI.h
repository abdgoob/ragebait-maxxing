#pragma once

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

// same shapes as the game, just with words on them now
class GameUI {
public:
    GameUI();
    explicit GameUI(const string& assetFolder);

    bool isReady() const;
    void setMouse(const sf::Vector2f& mouse);
    void box(sf::RenderTarget& target, float x, float y, float w, float h,
             const sf::Color& color) const;
    void text(sf::RenderTarget& target, const string& words, float x, float y,
              unsigned int size = 18, const sf::Color& color = sf::Color::White) const;
    void button(sf::RenderTarget& target, const string& words, float x, float y,
                float w, float h) const;
    void shade(sf::RenderTarget& target) const;
    void monitor(sf::RenderTarget& target, float x, float y, bool annoyed) const;

private:
    sf::Font font_;
    bool ready_;
    sf::Vector2f mouse_;
};
