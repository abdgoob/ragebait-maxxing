#pragma once

#include "ui/GameUI.h"

using namespace std;

// chapter 5 interaction, level code can use the dropped window as a platform
class PopupWindow {
public:
    PopupWindow();
    void reset();
    bool press(const sf::Vector2f& mouse);
    void move(const sf::Vector2f& mouse);
    void release();
    void draw(sf::RenderTarget& target, const GameUI& ui) const;

    bool isOpen() const;
    bool isPlatform() const;
    sf::FloatRect getPlatformBounds() const;

private:
    sf::Vector2f position_;
    sf::Vector2f dragOffset_;
    bool open_;
    bool dragging_;
    bool placed_;
    int closeDodges_;

    sf::FloatRect closeBounds() const;
};
