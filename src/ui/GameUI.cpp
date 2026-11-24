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

void GameUI::centeredText(sf::RenderTarget& target, const string& words, float x, float y,
                          unsigned int size, const sf::Color& color) const {
    if (!ready_) return;
    sf::Text label(words, font_, size);
    const sf::FloatRect bounds = label.getLocalBounds();
    label.setPosition(x - bounds.left - bounds.width * 0.5f, y - bounds.top);
    label.setFillColor(color);
    target.draw(label);
}

void GameUI::menuBackdrop(sf::RenderTarget& target) const {
    // just layers of rectangles and mountains, no extra scene system needed
    box(target, 0, 0, 960, 540, sf::Color(12, 25, 43));
    for (int i = 0; i < 18; ++i) {
        const float x = static_cast<float>((i * 137 + 33) % 940);
        const float y = static_cast<float>((i * 67 + 25) % 285);
        box(target, x, y, i % 3 == 0 ? 4.f : 2.f, 2, sf::Color(65, 118, 134));
    }
    // pixel moon behind the landscape
    box(target, 760, 64, 64, 48, sf::Color(107, 201, 194));
    box(target, 752, 72, 80, 32, sf::Color(107, 201, 194));
    box(target, 760, 64, 20, 16, sf::Color(153, 229, 208));
    box(target, 796, 82, 16, 12, sf::Color(78, 166, 172));
    for (int layer = 0; layer < 2; ++layer) {
        for (int i = 0; i < 7; ++i) {
            sf::ConvexShape mountain;
            mountain.setPointCount(3);
            const float x = i * 175.f - 90.f;
            const float y = 220.f + ((i * 43) % 100) + layer * 80.f;
            mountain.setPoint(0, sf::Vector2f(x, 500));
            mountain.setPoint(1, sf::Vector2f(x + 110, y));
            mountain.setPoint(2, sf::Vector2f(x + 240, 500));
            mountain.setFillColor(layer == 0 ? sf::Color(22, 57, 73) : sf::Color(18, 42, 58));
            target.draw(mountain);
        }
    }
    // side ruins keep the middle quiet enough to read the menu
    for (int side = 0; side < 2; ++side) {
        const float x = side == 0 ? 54.f : 790.f;
        box(target, x, 316, 108, 170, sf::Color(14, 29, 46));
        box(target, x + 8, 324, 92, 154, sf::Color(30, 53, 67));
        for (int row = 0; row < 4; ++row) {
            box(target, x + 16, 337.f + row * 33, 76, 25, sf::Color(17, 34, 49));
            box(target, x + 23, 345.f + row * 33, 5, 5, sf::Color(67, 181, 172));
            box(target, x + 34, 345.f + row * 33, 5, 5, sf::Color(166, 88, 144));
        }
    }
    box(target, 0, 482, 960, 58, sf::Color(35, 28, 49));
    box(target, 0, 482, 960, 6, sf::Color(78, 166, 151));
    box(target, 0, 488, 960, 8, sf::Color(43, 91, 92));
    for (int i = 0; i < 40; ++i) {
        box(target, i * 24.f, 488, 12, i % 3 == 0 ? 14.f : 7.f, sf::Color(43, 91, 92));
        box(target, i * 24.f + 4, 510.f + (i % 3) * 8.f, 6, 4, sf::Color(63, 46, 65));
    }
}

void GameUI::menuButton(sf::RenderTarget& target, const string& words, const string& key,
                        float x, float y, float w, float h, bool primary) const {
    const bool over = sf::FloatRect(x, y, w, h).contains(mouse_);
    const sf::Color edge = primary ? sf::Color(103, 234, 194) : sf::Color(114, 133, 160);
    const sf::Color face = primary ? (over ? sf::Color(52, 119, 111) : sf::Color(34, 83, 83)) :
        (over ? sf::Color(66, 76, 106) : sf::Color(39, 49, 72));
    // stepped corners and a bevel, like a chunky platform tile
    box(target, x + 4, y + 6, w - 8, h, sf::Color(7, 16, 28));
    box(target, x + 4, y, w - 8, h, edge);
    box(target, x, y + 4, w, h - 8, edge);
    box(target, x + 4, y + 4, w - 8, h - 8, face);
    box(target, x + 8, y + h - 7, w - 16, 3, sf::Color(20, 31, 47));
    box(target, x + 12, y + 10, 28, h - 20, sf::Color(13, 30, 42));
    centeredText(target, key, x + 26, y + 16, 12, edge);
    centeredText(target, words, x + w * 0.5f + 4, y + 13, 19,
                 primary ? sf::Color(206, 255, 231) : sf::Color(222, 229, 239));
    text(target, over ? ">>" : ">", x + w - 36, y + 14, 16, edge);
}
