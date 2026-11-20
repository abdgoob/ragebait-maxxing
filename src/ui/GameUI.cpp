#include "ui/GameUI.h"
#include <iostream>

using namespace std;

GameUI::GameUI() : GameUI("assets/") {}

GameUI::GameUI(const string& assetFolder) : ready_(false), mouse_(-1.f, -1.f) {
    ready_ = font_.loadFromFile(assetFolder + "fonts/DejaVuSansMono.ttf");
    if (!ready_) cerr << "couldnt load the font, keep assets next to the game\n";
}

bool GameUI::isReady() const { return ready_; }
void GameUI::setMouse(const sf::Vector2f& mouse) { mouse_ = mouse; }

void GameUI::box(sf::RenderTarget& target, float x, float y, float w, float h,
                 const sf::Color& color) const {
    sf::RectangleShape shape(sf::Vector2f(w, h));
    shape.setPosition(x, y);
    shape.setFillColor(color);
    target.draw(shape);
}

void GameUI::text(sf::RenderTarget& target, const string& words, float x, float y,
                  unsigned int size, const sf::Color& color) const {
    if (!ready_) return;
    sf::Text label(words, font_, size);
    label.setPosition(x, y);
    label.setFillColor(color);
    target.draw(label);
}

void GameUI::button(sf::RenderTarget& target, const string& words, float x, float y,
                    float w, float h) const {
    const bool over = sf::FloatRect(x, y, w, h).contains(mouse_);
    box(target, x, y, w, h, over ? sf::Color(110, 80, 160) : sf::Color(65, 48, 95));
    text(target, words, x + 18.f, y + 12.f, 16);
}

void GameUI::shade(sf::RenderTarget& target) const {
    box(target, 0.f, 0.f, 960.f, 540.f, sf::Color(12, 10, 20, 225));
}

void GameUI::monitor(sf::RenderTarget& target, float x, float y, bool annoyed) const {
    box(target, x, y, 48.f, 36.f, sf::Color(95, 78, 130));
    box(target, x + 4.f, y + 4.f, 40.f, 28.f, sf::Color(25, 22, 35));
    const sf::Color face = annoyed ? sf::Color(255, 110, 130) : sf::Color(120, 240, 190);
    box(target, x + 12.f, y + 11.f, 5.f, annoyed ? 3.f : 6.f, face);
    box(target, x + 31.f, y + 11.f, 5.f, annoyed ? 3.f : 6.f, face);
    box(target, x + 18.f, y + 24.f, 12.f, 2.f, face);
    box(target, x + 20.f, y + 36.f, 8.f, 5.f, sf::Color(95, 78, 130));
}
